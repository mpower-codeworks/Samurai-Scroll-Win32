/*
    / / / / / / / / / / / / / /
  / / modSamuraiRender.c  / /
/ / / / / / / / / / / / / /

all GDI frame drawing

*/

#include "hFiles/modSamuraiRender.h"
#include "hFiles/modSamuraiAssets.h"
#include "hFiles/modFonts.h"
#include "hFiles/modSamuraiGame.h"
#include "hFiles/modSamuraiWindow.h"

/* =============================
** fullscreen title help message
** =============================
*/
#define FULLTITLE_HELP_BASE_W          900
#define FULLTITLE_HELP_BASE_H          600

#define FULLTITLE_HELP_CLICK_X         190
#define FULLTITLE_HELP_CLICK_Y         536
#define FULLTITLE_HELP_CLICK_W         520
#define FULLTITLE_HELP_CLICK_H          32

#define FULLTITLE_HELP_FULLSCREEN_X    190
#define FULLTITLE_HELP_FULLSCREEN_Y    556
#define FULLTITLE_HELP_FULLSCREEN_W    520
#define FULLTITLE_HELP_FULLSCREEN_H     32

#define FULLTITLE_HELP_ESC_X           190
#define FULLTITLE_HELP_ESC_Y           576
#define FULLTITLE_HELP_ESC_W           520
#define FULLTITLE_HELP_ESC_H            32

#define FULLTITLE_HELP_FONT_MIN         14
#define FULLTITLE_HELP_FONT_MAX         26

/* ==================
** gameplay hud help
** ==================
*/
#define GAMEPLAY_HELP_FONT_MIN          22
#define GAMEPLAY_HELP_FONT_MAX          34
#define GAMEPLAY_HELP_PANEL_ROWS         5
#define GAMEPLAY_HELP_TOP_NUDGE          2

/* ==========
** prototypes
** ==========
*/
static void RenderTitle (
    HDC dc
);
static void RenderGame (
    HDC dc
);
static void DrawFullscreenTitle (
    HDC dc,
    int cw,
    int ch
);
static void DrawFullscreenTitleHelp (
    HDC dc,
    int out_x,
    int out_y,
    int out_w,
    int out_h,
    int title_h
);
static void SetFullscreenTitleHelpRect (
    RECT *r,
    int   out_x,
    int   out_y,
    int   out_w,
    int   out_h,
    int   base_x,
    int   base_y,
    int   base_w,
    int   base_h
);
static void DrawFullscreenHud (
    HDC dc,
    int cw,
    int ch,
    int game_x,
    int game_y,
    int game_size
);
static void DrawTextShadow (
    HDC        dc,
    const char *text,
    RECT       *r,
    UINT       format,
    COLORREF   text_color,
    COLORREF   shadow_color
);
static void DrawPanelBorder (
    HDC  dc,
    RECT *panel
);
static void DrawBitmapFit (
    HDC dc,
    int bmp_index,
    int x,
    int y,
    int w,
    int h
);
static void DrawTile (
    HDC  dc,
    char ch,
    int  x,
    int  y
);
static int TileIndexForChar (
    char ch
);

/* ============
** render frame
** ============
*/
void Render (
    HWND hwnd,
    HDC  hdc)
{
    /* local vars
    */
    RECT client;
    int cw;
    int ch;
    int scale_x;
    int scale_y;
    int scale;
    int out_w;
    int out_h;
    int out_x;
    int out_y;

    /* check back buffer
    */
    if (!g_back_dc) {
        return;
    }

    /* draw logical frame
    */
    if (g_on_title) {
        RenderTitle(g_back_dc);
    } else {
        RenderGame(g_back_dc);
    }

    /* get client size
    */
    GetClientRect (
        hwnd,
        &client
    );
    cw = client.right - client.left;
    ch = client.bottom - client.top;

    /* make present buffer
    */
    if (!EnsurePresentBuffer(hdc, cw, ch)) {
        return;
    }

    FillRect (
        g_present_dc,
        &client,
        (HBRUSH)GetStockObject(BLACK_BRUSH)
    );

    SetStretchBltMode (
        g_present_dc,
        COLORONCOLOR
    );

    /* fullscreen title
    */
    if (g_fullscreen && g_on_title) {
        DrawFullscreenTitle (
            g_present_dc,
            cw,
            ch
        );
        BitBlt (
            hdc,
            0,
            0,
            cw,
            ch,
            g_present_dc,
            0,
            0,
            SRCCOPY
        );
        return;
    }

    /* fullscreen game
    */
    if (g_fullscreen && !g_on_title) {
        int source_x;
        int source_y;
        int source_w;
        int source_h;
        int out_size;

        source_x = 12;
        source_y = 12;
        source_w = VIEW_TILES * TILE;
        source_h = VIEW_TILES * TILE;

        out_size = cw < ch ? cw : ch;
        out_w = out_size;
        out_h = out_size;
        out_x = (cw - out_w) / 2;
        out_y = (ch - out_h) / 2;

        StretchBlt (
            g_present_dc,
            out_x,
            out_y,
            out_w,
            out_h,
            g_back_dc,
            source_x,
            source_y,
            source_w,
            source_h,
            SRCCOPY
        );

        DrawFullscreenHud (
            g_present_dc,
            cw,
            ch,
            out_x,
            out_y,
            out_size
        );

        BitBlt (
            hdc,
            0,
            0,
            cw,
            ch,
            g_present_dc,
            0,
            0,
            SRCCOPY
        );
        return;
    }

    /* windowed scaling
    */
    scale_x = cw / LOGICAL_W;
    scale_y = ch / LOGICAL_H;
    scale = scale_x < scale_y ? scale_x : scale_y;
    if (scale < 1) {
        scale = 1;
    }

    out_w = LOGICAL_W * scale;
    out_h = LOGICAL_H * scale;
    if (out_w > cw || out_h > ch) {
        if (cw * LOGICAL_H < ch * LOGICAL_W) {
            out_w = cw;
            out_h = (cw * LOGICAL_H) / LOGICAL_W;
        } else {
            out_h = ch;
            out_w = (ch * LOGICAL_W) / LOGICAL_H;
        }
    }

    out_x = (cw - out_w) / 2;
    out_y = (ch - out_h) / 2;

    StretchBlt (
        g_present_dc,
        out_x,
        out_y,
        out_w,
        out_h,
        g_back_dc,
        0,
        0,
        LOGICAL_W,
        LOGICAL_H,
        SRCCOPY
    );

    BitBlt (
        hdc,
        0,
        0,
        cw,
        ch,
        g_present_dc,
        0,
        0,
        SRCCOPY
    );
}

/* ================
** draw shadow text
** ================
*/
static void DrawTextShadow (
    HDC       dc,
    const char *text,
    RECT      *r,
    UINT       format,
    COLORREF   text_color,
    COLORREF   shadow_color)
{
    /* local vars
    */
    RECT shadow_r;

    shadow_r = *r;
    OffsetRect (
        &shadow_r,
        2,
        2
    );
    SetTextColor (
        dc,
        shadow_color
    );
    DrawTextA (
        dc,
        text,
        -1,
        &shadow_r,
        format
    );

    SetTextColor (
        dc,
        text_color
    );
    DrawTextA (
        dc,
        text,
        -1,
        r,
        format
    );
}

/* =================
** draw panel border
** =================
*/
static void DrawPanelBorder (
    HDC  dc,
    RECT *panel)
{
    /* local vars
    */
    HPEN   pen;
    HPEN   old_pen;
    HBRUSH old_brush;

    /* create border pen
    */
    pen = CreatePen (
        PS_SOLID,
        1,
        RGB(40, 40, 40)
    );

    /* select objects
    */
    old_pen = (HPEN)SelectObject (
        dc,
        pen
    );
    old_brush = (HBRUSH)SelectObject (
        dc,
        GetStockObject(NULL_BRUSH)
    );

    /* draw rectangle
    */
    Rectangle (
        dc,
        panel->left,
        panel->top,
        panel->right,
        panel->bottom
    );

    /* restore objects
    */
    SelectObject (
        dc,
        old_brush
    );
    SelectObject (
        dc,
        old_pen
    );

    /* free pen
    */
    DeleteObject(pen);
}

/* =====================
** draw fullscreen title
** =====================
*/
static void DrawFullscreenTitle (
    HDC dc,
    int cw,
    int ch)
{
    /* local vars
    */
    int bmp_w;
    int bmp_h;
    int out_w;
    int out_h;
    int out_x;
    int out_y;
    int title_h;
    RECT r;

    /* check size
    */
    if (cw <= 0 || ch <= 0) {
        return;
    }

    /* clear screen
    */
    r.left = 0;
    r.top = 0;
    r.right = cw;
    r.bottom = ch;
    FillRect (
        dc,
        &r,
        (HBRUSH)GetStockObject(BLACK_BRUSH)
    );

    /* default title area
    */
    out_x = 0;
    out_y = 0;
    out_w = cw;
    out_h = ch;

    /* draw title bitmap
    */
    if (
        g_bmp[BMP_TITLE].bmp &&
        g_bmp[BMP_TITLE].dc) {
        bmp_w = g_bmp[BMP_TITLE].w;
        bmp_h = g_bmp[BMP_TITLE].h;

        if (cw * bmp_h < ch * bmp_w) {
            out_w = cw;
            out_h = (cw * bmp_h) / bmp_w;
        } else {
            out_h = ch;
            out_w = (ch * bmp_w) / bmp_h;
        }

        out_x = (cw - out_w) / 2;
        out_y = (ch - out_h) / 2;

        DrawBitmapFit (
            dc,
            BMP_TITLE,
            out_x,
            out_y,
            out_w,
            out_h
        );
    }

    /* size help font
    */
    title_h = ch / FONT_FULLSCREEN_TITLE_DIV;
    if (title_h < FONT_FULLSCREEN_TITLE_MIN) {
        title_h = FONT_FULLSCREEN_TITLE_MIN;
    }
    if (title_h > FONT_FULLSCREEN_TITLE_MAX) {
        title_h = FONT_FULLSCREEN_TITLE_MAX;
    }

    /* draw help text
    */
    DrawFullscreenTitleHelp (
        dc,
        out_x,
        out_y,
        out_w,
        out_h,
        title_h
    );
}

/* ==========================
** draw fullscreen title help
** ==========================
*/
static void DrawFullscreenTitleHelp (
    HDC dc,
    int out_x,
    int out_y,
    int out_w,
    int out_h,
    int title_h)
{
    /* local vars
    */
    RECT  r_click;       // start text area
    RECT  r_fullscreen;  // fullscreen text area
    RECT  r_esc;         // quit text area
    int   help_h;
    HFONT font_help;
    HFONT old_font;

    /* size font
    */
    help_h = (title_h * 3) / 4;
    if (help_h < FULLTITLE_HELP_FONT_MIN) {
        help_h = FULLTITLE_HELP_FONT_MIN;
    }
    if (help_h > FULLTITLE_HELP_FONT_MAX) {
        help_h = FULLTITLE_HELP_FONT_MAX;
    }

    /* create font
    */
    font_help = CreateFontWithFallback (
        -help_h,
        FW_BOLD,
        ANTIALIASED_QUALITY,
        "Tahoma"
    );

    /* select font
    */
    old_font = (HFONT)SelectObject (
        dc,
        font_help ? font_help :
        GetStockObject(DEFAULT_GUI_FONT)
    );

    SetBkMode (
        dc,
        TRANSPARENT
    );

    /* start text
    */
    SetFullscreenTitleHelpRect (
        &r_click,
        out_x,
        out_y,
        out_w,
        out_h,
        FULLTITLE_HELP_CLICK_X,
        FULLTITLE_HELP_CLICK_Y,
        FULLTITLE_HELP_CLICK_W,
        FULLTITLE_HELP_CLICK_H
    );
    DrawTextShadow (
        dc,
        "Click, press ENTER, or press SPACE to begin.",
        &r_click,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE,
        RGB(75, 75, 75),
        RGB(220, 220, 220)
    );

    /* fullscreen text
    */
    SetFullscreenTitleHelpRect (
        &r_fullscreen,
        out_x,
        out_y,
        out_w,
        out_h,
        FULLTITLE_HELP_FULLSCREEN_X,
        FULLTITLE_HELP_FULLSCREEN_Y,
        FULLTITLE_HELP_FULLSCREEN_W,
        FULLTITLE_HELP_FULLSCREEN_H
    );
    DrawTextShadow (
        dc,
        "F11 toggles full screen.",
        &r_fullscreen,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE,
        RGB(75, 75, 75),
        RGB(220, 220, 220)
    );

    /* esc text
    */
    SetFullscreenTitleHelpRect (
        &r_esc,
        out_x,
        out_y,
        out_w,
        out_h,
        FULLTITLE_HELP_ESC_X,
        FULLTITLE_HELP_ESC_Y,
        FULLTITLE_HELP_ESC_W,
        FULLTITLE_HELP_ESC_H
    );
    DrawTextShadow (
        dc,
        "ESC quits.",
        &r_esc,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE,
        RGB(75, 75, 75),
        RGB(220, 220, 220)
    );

    /* restore font
    */
    SelectObject (
        dc,
        old_font
    );

    /* free font
    */
    FreeFallbackFont (
        font_help
    );
}

/* ==============================
** set fullscreen title help rect
** ==============================
*/
static void SetFullscreenTitleHelpRect (
    RECT *r,
    int   out_x,
    int   out_y,
    int   out_w,
    int   out_h,
    int   base_x,
    int   base_y,
    int   base_w,
    int   base_h)
{
    /* scale rect
    */
    r->left = out_x +
        (out_w * base_x) / FULLTITLE_HELP_BASE_W;
    r->top = out_y +
        (out_h * base_y) / FULLTITLE_HELP_BASE_H;
    r->right = out_x +
        (out_w * (base_x + base_w)) / FULLTITLE_HELP_BASE_W;
    r->bottom = out_y +
        (out_h * (base_y + base_h)) / FULLTITLE_HELP_BASE_H;
}

/* ===================
** draw fullscreen hud
** ===================
*/
static void DrawFullscreenHud (
    HDC dc,
    int cw,
    int ch,
    int game_x,
    int game_y,
    int game_size)
{
    /* local vars
    */
    int side_w;
    int margin;
    int pad;
    int text_h;
    int small_h;
    int help_h;
    int big_h;
    int equip_h;
    int i;
    int start_line;
    int right_left;
    int right_right;
    char text[128];
    char logbuf[4096];
    RECT r;
    RECT panel;
    HFONT old_font;
    HFONT font_text;
    HFONT font_label;
    HFONT font_timer;
    HFONT font_equip;
    HFONT font_small;
    HFONT font_help;
    HPEN pen;
    HPEN old_pen;
    HBRUSH brush;
    HBRUSH old_brush;
    COLORREF timer_color;

    /* check size
    */
    if (
        cw <= 0 ||
        ch <= 0 ||
        game_size <= 0) {
        return;
    }

    /* compute layout
    */
    side_w = game_x;

    margin = cw / 80;
    if (margin < 18) {
        margin = 18;
    }
    if (margin > 36) {
        margin = 36;
    }

    pad = margin / 2;
    if (pad < 8) {
        pad = 8;
    }

    text_h = ch / FONT_FULLSCREEN_TEXT_DIV;
    if (text_h < FONT_FULLSCREEN_TEXT_MIN) {
        text_h = FONT_FULLSCREEN_TEXT_MIN;
    }
    if (text_h > FONT_FULLSCREEN_TEXT_MAX) {
        text_h = FONT_FULLSCREEN_TEXT_MAX;
    }

    small_h = ch / FONT_FULLSCREEN_SMALL_DIV;
    if (small_h < FONT_FULLSCREEN_SMALL_MIN) {
        small_h = FONT_FULLSCREEN_SMALL_MIN;
    }
    if (small_h > FONT_FULLSCREEN_SMALL_MAX) {
        small_h = FONT_FULLSCREEN_SMALL_MAX;
    }

    /* make gameplay help larger
    */
    help_h = text_h + (text_h / 4);
    if (help_h < GAMEPLAY_HELP_FONT_MIN) {
        help_h = GAMEPLAY_HELP_FONT_MIN;
    }
    if (help_h > GAMEPLAY_HELP_FONT_MAX) {
        help_h = GAMEPLAY_HELP_FONT_MAX;
    }

    big_h = ch / FONT_FULLSCREEN_TIMER_DIV;
    if (big_h < FONT_FULLSCREEN_TIMER_MIN) {
        big_h = FONT_FULLSCREEN_TIMER_MIN;
    }
    if (big_h > FONT_FULLSCREEN_TIMER_MAX) {
        big_h = FONT_FULLSCREEN_TIMER_MAX;
    }

    equip_h = ch / FONT_FULLSCREEN_EQUIP_DIV;
    if (equip_h < FONT_FULLSCREEN_EQUIP_MIN) {
        equip_h = FONT_FULLSCREEN_EQUIP_MIN;
    }
    if (equip_h > FONT_FULLSCREEN_EQUIP_MAX) {
        equip_h = FONT_FULLSCREEN_EQUIP_MAX;
    }

    /* create hud fonts
    */
    font_text = CreateFontWithFallback (
        -text_h,
        FW_NORMAL,
        ANTIALIASED_QUALITY,
        "Tahoma"
    );

    font_label = CreateFontWithFallback (
        -text_h,
        FW_BOLD,
        ANTIALIASED_QUALITY,
        "Tahoma"
    );

    font_timer = CreateFontWithFallback (
        -big_h,
        FW_BOLD,
        ANTIALIASED_QUALITY,
        "Tahoma"
    );

    font_equip = CreateFontWithFallback (
        -equip_h,
        FW_BOLD,
        ANTIALIASED_QUALITY,
        "Tahoma"
    );

    font_small = CreateFontWithFallback (
        -small_h,
        FW_BOLD,
        ANTIALIASED_QUALITY,
        "Tahoma"
    );

    font_help = CreateFontWithFallback (
        -help_h,
        FW_BOLD,
        ANTIALIASED_QUALITY,
        "Tahoma"
    );

    old_font = (HFONT)SelectObject (
        dc,
        font_text ? font_text :
        GetStockObject(DEFAULT_GUI_FONT)
    );
    SetBkMode (
        dc,
        TRANSPARENT
    );

    /* draw game border
    */
    pen = CreatePen (
        PS_SOLID,
        1,
        RGB(56, 56, 56)
    );
    old_pen = (HPEN)SelectObject (
        dc,
        pen
    );
    old_brush = (HBRUSH)SelectObject (
        dc,
        GetStockObject(NULL_BRUSH)
    );
    Rectangle (
        dc,
        game_x,
        game_y,
        game_x + game_size + 1,
        game_y + game_size + 1
    );
    SelectObject (
        dc,
        old_brush
    );
    SelectObject (
        dc,
        old_pen
    );
    DeleteObject(pen);

    /* draw messages panel
    */
    if (side_w >= 160) {
        panel.left = margin;
        panel.top = margin;
        panel.right = game_x - margin;
        panel.bottom = ch - margin;

        brush = CreateSolidBrush(RGB(8, 8, 8));
        FillRect (
            dc,
            &panel,
            brush
        );
        DeleteObject(brush);

        /* draw panel border
        */
        DrawPanelBorder (
            dc,
            &panel
        );

        SelectObject (
            dc,
            font_small ? font_small :
            GetStockObject(DEFAULT_GUI_FONT)
        );
        r.left = panel.left + pad;
        r.top = panel.top + pad;
        r.right = panel.right - pad;
        r.bottom = r.top + small_h + 8;
        DrawTextShadow (
            dc,
            "MESSAGES",
            &r,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE,
            RGB(90, 170, 255),
            RGB(0, 0, 0)
        );

        logbuf[0] = 0;
        start_line =
            g_log_count > 12 ? g_log_count - 12 : 0;
        for (i = start_line; i < g_log_count; i++) {
            if (
                lstrlenA(logbuf) +
                lstrlenA(g_log[i]) +
                4 < (int)sizeof(logbuf)) {
                lstrcatA (
                    logbuf,
                    g_log[i]
                );
                lstrcatA (
                    logbuf,
                    "\r\n\r\n"
                );
            }
        }

        SelectObject (
            dc,
            font_text ? font_text :
            GetStockObject(DEFAULT_GUI_FONT)
        );
        r.left   = panel.left   + pad;
        r.top    = panel.top    + pad + small_h + 12;
        r.right  = panel.right  - pad;
        r.bottom = panel.bottom - pad;
        DrawTextShadow (
            dc,
            logbuf,
            &r,
            DT_LEFT | DT_WORDBREAK | DT_EDITCONTROL,
            RGB(214, 232, 255),
            RGB(0, 0, 0)
        );
    }

    /* draw right panel
    */
    right_left = game_x + game_size + margin;
    right_right = cw - margin;

    if (right_right - right_left >= 150) {
        if (g_talked_to_sensei) {
            panel.left = right_left;
            panel.top = margin;
            panel.right = right_right;
            panel.bottom = margin + (ch / 4);

            brush = CreateSolidBrush(RGB(8, 8, 8));
            FillRect (
                dc,
                &panel,
                brush
            );
            DeleteObject(brush);

            /* draw panel border
            */
            DrawPanelBorder (
                dc,
                &panel
            );

            SelectObject (
                dc,
                font_small ? font_small :
                GetStockObject(DEFAULT_GUI_FONT)
            );
            r.left = panel.left + pad;
            r.top = panel.top + pad;
            r.right = panel.right - pad;
            r.bottom = r.top + (small_h * 2) + 8;
            DrawTextShadow (
                dc,
                "SENSEI LIFE\r\nREMAINING",
                &r,
                DT_CENTER | DT_WORDBREAK,
                RGB(90, 170, 255),
                RGB(0, 0, 0)
            );

            if (g_time_left <= 10) {
                timer_color = RGB(255, 96, 96);
            } else {
                timer_color = RGB(245, 248, 255);
            }

            wsprintfA (
                text,
                "%d",
                g_time_left < 0 ? 0 : g_time_left
            );
            SelectObject (
                dc,
                font_timer ? font_timer :
                GetStockObject(DEFAULT_GUI_FONT)
            );
            r.left = panel.left + pad;
            r.top = panel.top + pad + (small_h * 2) + 4;
            r.right = panel.right - pad;
            r.bottom = panel.bottom - pad;
            DrawTextShadow (
                dc,
                text,
                &r,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                timer_color,
                RGB(0, 0, 0)
            );
        }

        /* draw help panel
        */
        if (IsGameplayHelpVisible()) {
            panel.left = right_left;
            panel.right = right_right;
            panel.bottom = ch - margin - (ch / 4) - margin;
            panel.top =
                panel.bottom -
                ((help_h * GAMEPLAY_HELP_PANEL_ROWS) +
                (pad * 2));

            brush = CreateSolidBrush(RGB(8, 8, 8));
            FillRect (
                dc,
                &panel,
                brush
            );
            DeleteObject(brush);

            /* draw panel border
            */
            DrawPanelBorder (
                dc,
                &panel
            );

            SelectObject (
                dc,
                font_help ? font_help :
                GetStockObject(DEFAULT_GUI_FONT)
            );
            r.left = panel.left + pad;
            r.top =
                panel.top +
                pad +
                GAMEPLAY_HELP_TOP_NUDGE;
            r.right = panel.right - pad;
            r.bottom = panel.bottom - pad;
            DrawTextShadow (
                dc,
                "Number Pad / Arrow Keys to move\r\n"
                "T to talk\r\n"
                "ESC to title",
                &r,
                DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL,
                RGB(245, 248, 255),
                RGB(0, 0, 0)
            );
        }

        /* draw equipped panel
        */
        panel.left = right_left;
        panel.top = ch - margin - (ch / 4);
        panel.right = right_right;
        panel.bottom = ch - margin;

        brush = CreateSolidBrush(RGB(8, 8, 8));
        FillRect (
            dc,
            &panel,
            brush
        );
        DeleteObject(brush);

        /* draw panel border
        */
        DrawPanelBorder (
            dc,
            &panel
        );

        SelectObject (
            dc,
            font_small ? font_small :
            GetStockObject(DEFAULT_GUI_FONT)
        );
        r.left = panel.left + pad;
        r.top = panel.top + pad;
        r.right = panel.right - pad;
        r.bottom = r.top + small_h + 8;
        DrawTextShadow (
            dc,
            "EQUIPPED",
            &r,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE,
            RGB(90, 170, 255),
            RGB(0, 0, 0)
        );

        SelectObject (
            dc,
            font_equip ? font_equip :
            GetStockObject(DEFAULT_GUI_FONT)
        );
        r.left = panel.left + pad;
        r.top = panel.top + pad + small_h + 12;
        r.right = panel.right - pad;
        r.bottom = panel.bottom - pad;
        if (g_found_scroll) {
            DrawTextShadow (
                dc,
                "Scroll",
                &r,
                DT_CENTER | DT_WORDBREAK | DT_VCENTER,
                RGB(245, 248, 255),
                RGB(0, 0, 0)
            );
        } else {
            DrawTextShadow (
                dc,
                "Nothing",
                &r,
                DT_CENTER | DT_WORDBREAK | DT_VCENTER,
                RGB(180, 190, 205),
                RGB(0, 0, 0)
            );
        }
    }

    /* cleanup fonts
    */
    SelectObject (
        dc,
        old_font
    );

    FreeFallbackFont (
        font_text
    );
    FreeFallbackFont (
        font_label
    );
    FreeFallbackFont (
        font_timer
    );
    FreeFallbackFont (
        font_equip
    );
    FreeFallbackFont (
        font_small
    );
    FreeFallbackFont (
        font_help
    );
}

/* ============
** render title
** ============
*/
static void RenderTitle (
    HDC dc)
{
    /* local vars
    */
    RECT r;
    HBRUSH white;
    HFONT old_font;

    white = CreateSolidBrush (
        RGB(255, 255, 255)
    );
    /* clear background
    */
    r.left = 0;
    r.top = 0;
    r.right = LOGICAL_W;
    r.bottom = LOGICAL_H;
    FillRect (
        dc,
        &r,
        white
    );
    DeleteObject(white);

    DrawBitmapFit (
        dc,
        BMP_TITLE,
        102,
        40,
        300,
        200
    );

    SetBkMode (
        dc,
        TRANSPARENT
    );
    SetTextColor (
        dc,
        RGB(0, 0, 0)
    );
    old_font = (HFONT)SelectObject (
        dc,
        g_font_big
    );
    /* clear background
    */
    r.left = 0;
    r.top = 250;
    r.right = LOGICAL_W;
    r.bottom = 290;
    DrawTextA (
        dc,
        "SAMURAI SCROLL",
        -1,
        &r,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE
    );

    SelectObject (
        dc,
        g_font_small
    );
    r.left = 40;
    r.top = 298;
    r.right = LOGICAL_W - 40;
    r.bottom = 350;
    DrawTextA (
        dc,
        "Click, press ENTER, or press SPACE to begin."
        "\nF11 toggles fullscreen. ESC quits.",
        -1,
        &r,
        DT_CENTER | DT_WORDBREAK
    );

    SelectObject (
        dc,
        old_font
    );
}

/* ===========
** render game
** ===========
*/
static void RenderGame (
    HDC dc)
{
    /* local vars
    */
    RECT r;
    HBRUSH white;
    HBRUSH black;
    HPEN frame_pen;
    HPEN old_pen;
    HBRUSH old_brush;
    HFONT old_font;
    int y;
    int x;
    int i;
    char text[128];
    char logbuf[4096];
    int start_line;

    white = CreateSolidBrush (
        RGB(255, 255, 255)
    );
    /* clear background
    */
    r.left   = 0;
    r.top    = 0;
    r.right  = LOGICAL_W;
    r.bottom = LOGICAL_H;
    FillRect (
        dc,
        &r,
        white
    );
    DeleteObject(white);

    /* draw the original 7x7 neighborhood view
    */
    for (y = -3; y <= 3; y++) {
        for (x = -3; x <= 3; x++) {
            int sx;
            int sy;
            char ch;

            sx = 12 + (x + 3) * TILE;
            sy = 12 + (y + 3) * TILE;

            if (x == 0 && y == 0) {
                continue;
            }

            if (y + g_char_y < 1 ||
                x + g_char_x < 1 ||
                x + g_char_x > MAP_W ||
                y + g_char_y > MAP_H) {
                DrawBitmapFit (
                    dc,
                    BMP_BLACK,
                    sx,
                    sy,
                    TILE,
                    TILE
                );
            } else {
                ch = GetCell (
                    y + g_char_y + 1,
                    x + g_char_x + 1
                );
                DrawTile (
                    dc,
                    ch,
                    sx,
                    sy
                );
            }
        }
    }

    if (g_char_face == 1) {
        DrawBitmapFit (
            dc,
            BMP_DOWNG,
            12 + 3 * TILE,
            12 + 3 * TILE,
            TILE,
            TILE
        );
    } else if (g_char_face == 2) {
        DrawBitmapFit (
            dc,
            BMP_UPG,
            12 + 3 * TILE,
            12 + 3 * TILE,
            TILE,
            TILE
        );
    } else if (g_char_face == 3) {
        DrawBitmapFit (
            dc,
            BMP_LEFTG,
            12 + 3 * TILE,
            12 + 3 * TILE,
            TILE,
            TILE
        );
    } else {
        DrawBitmapFit (
            dc,
            BMP_RIGHTG,
            12 + 3 * TILE,
            12 + 3 * TILE,
            TILE,
            TILE
        );
    }

    frame_pen = CreatePen (
        PS_SOLID,
        1,
        RGB(0, 0, 0)
    );
    old_pen = (HPEN)SelectObject (
        dc,
        frame_pen
    );
    old_brush = (HBRUSH)SelectObject (
        dc,
        GetStockObject(NULL_BRUSH)
    );
    Rectangle (
        dc,
        11,
        11,
        12 + VIEW_TILES * TILE + 1,
        12 + VIEW_TILES * TILE + 1
    );
    SelectObject (
        dc,
        old_brush
    );
    SelectObject (
        dc,
        old_pen
    );
    DeleteObject(frame_pen);

    DrawBitmapFit (
        dc,
        BMP_TITLE,
        290,
        12,
        190,
        127
    );

    SetBkMode (
        dc,
        TRANSPARENT
    );
    old_font = (HFONT)SelectObject (
        dc,
        g_font_big
    );
    SetTextColor (
        dc,
        RGB(0, 0, 0)
    );
    r.left = 276;
    r.top = 145;
    r.right = 492;
    r.bottom = 205;
    if (g_found_scroll) {
        DrawTextA (
            dc,
            "Equipment:\nScroll",
            -1,
            &r,
            DT_CENTER | DT_WORDBREAK
        );
    } else {
        DrawTextA (
            dc,
            "Equipment:\nNothing",
            -1,
            &r,
            DT_CENTER | DT_WORDBREAK
        );
    }

    if (g_talked_to_sensei) {
        SelectObject (
            dc,
            g_font_small
        );
        SetTextColor (
            dc,
            RGB(0, 0, 255)
        );
        r.left = 340;
        r.top = 213;
        r.right = 448;
        r.bottom = 248;
        DrawTextA (
            dc,
            "Sensei Life\nRemaining",
            -1,
            &r,
            DT_CENTER | DT_WORDBREAK
        );

        SelectObject (
            dc,
            g_font_big
        );
        wsprintfA (
            text,
            "%d",
            g_time_left < 0 ? 0 : g_time_left
        );
        r.left = 448;
        r.top = 207;
        r.right = 492;
        r.bottom = 250;
        FillRect (
            dc,
            &r,
            (HBRUSH)GetStockObject(WHITE_BRUSH)
        );
        Rectangle (
            dc,
            r.left,
            r.top,
            r.right,
            r.bottom
        );
        SetTextColor (
            dc,
            RGB(0, 0, 255)
        );
        DrawTextA (
            dc,
            text,
            -1,
            &r,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE
        );
    }

    black = CreateSolidBrush(RGB(0, 0, 0));
    r.left = 12;
    r.top = 268;
    r.right = 492;
    r.bottom = 382;
    FillRect (
        dc,
        &r,
        black
    );
    DeleteObject(black);

    frame_pen = CreatePen (
        PS_SOLID,
        1,
        RGB(0, 0, 255)
    );
    SelectObject (
        dc,
        frame_pen
    );
    Rectangle (
        dc,
        12,
        268,
        492,
        382
    );
    DeleteObject(frame_pen);

    logbuf[0] = 0;
    start_line = g_log_count > 10 ? g_log_count - 10 : 0;
    for (i = start_line; i < g_log_count; i++) {
        if (
            lstrlenA(logbuf)   +
            lstrlenA(g_log[i]) +
            4 < (int)sizeof(logbuf)) {
            lstrcatA (
                logbuf,
                g_log[i]
            );
            lstrcatA (
                logbuf,
                "\r\n"
            );
        }
    }

    SelectObject (
        dc,
        g_font_ui
    );
    SetTextColor (
        dc,
        RGB(0, 128, 255)
    );
    r.left = 18;
    r.top = 274;
    r.right = 486;
    r.bottom = 376;
    DrawTextA (
        dc,
        logbuf,
        -1,
        &r,
        DT_WORDBREAK | DT_EDITCONTROL
    );

    SelectObject (
        dc,
        g_font_small
    );
    SetTextColor (
        dc,
        RGB(0, 0, 0)
    );
    /* clear background
    */
    r.left = 0;
    r.top = 383;
    r.right = LOGICAL_W;
    r.bottom = LOGICAL_H;
    DrawTextA (
        dc,
        "Number Pad / Arrow Keys to move    "
        "T to talk    ESC to title",
        -1,
        &r,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE
    );

    SelectObject (
        dc,
        old_font
    );
}

/* ===============
** draw bitmap fit
** ===============
*/
static void DrawBitmapFit (
    HDC dc,
    int bmp_index,
    int x,
    int y,
    int w,
    int h)
{
    /* local vars
    */
    HBRUSH brush;
    RECT r;

    if (bmp_index < 0 ||
        bmp_index >= BMP_COUNT ||
        !g_bmp[bmp_index].bmp ||
        !g_bmp[bmp_index].dc) {
        r.left = x;
        r.top = y;
        r.right = x + w;
        r.bottom = y + h;
        brush = CreateSolidBrush (
            RGB(255, 0, 255)
        );
        FillRect (
            dc,
            &r,
            brush
        );
        DeleteObject(brush);
        return;
    }

    SetStretchBltMode (
        dc,
        COLORONCOLOR
    );
    /* stretch bitmap
    */
    StretchBlt (
        dc,
        x,
        y,
        w,
        h,
        g_bmp[bmp_index].dc,
        0,
        0,
        g_bmp[bmp_index].w,
        g_bmp[bmp_index].h,
        SRCCOPY
    );
}

/* =========
** draw tile
** =========
*/
static void DrawTile (
    HDC  dc,
    char ch,
    int  x,
    int  y)
{
    /* local vars
    */
    DrawBitmapFit (
        dc,
        TileIndexForChar(ch),
        x,
        y,
        TILE,
        TILE
    );
}

/* =============
** tile for char
** =============
*/
static int TileIndexForChar (
    char ch)
{
    /* local vars
    */
    switch (ch) {
    case '0': return BMP_BB;
    case '1': return BMP_BL;
    case '2': return BMP_BR;
    case '3': return BMP_BRIDGE;
    case '4': return BMP_BUSH;
    case '5': return BMP_FOREST;
    case '6': return BMP_GRASS;
    case '7': return BMP_GUY1;
    case '8': return BMP_GUY2;
    case '9': return BMP_GUY3;
    case 'A': return BMP_GUY4;
    case 'a': return BMP_GUY4;
    case 'B': return BMP_GUY5;
    case 'C': return BMP_ILB;
    case 'D': return BMP_ILT;
    case 'E': return BMP_IRB;
    case 'F': return BMP_IRT;
    case 'G': return BMP_L;
    case 'H': return BMP_MOUNT1;
    case 'I': return BMP_MOUNT2;
    case 'J': return BMP_MOUNT3;
    case 'K': return BMP_MOUNT4;
    case 'L': return BMP_MOUNTAIN;
    case 'M': return BMP_R;
    case 'N': return BMP_SCROLL;
    case 'O': return BMP_SIGN;
    case 'P': return BMP_SKULL;
    case 'Q': return BMP_TL;
    case 'R': return BMP_TORI;
    case 'S': return BMP_TR;
    case 'T': return BMP_TT;
    case 'U': return BMP_VILLAGE;
    case 'V': return BMP_BLACK;
    default:  return BMP_BLACK;
    }
}
