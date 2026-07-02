/*
    / / / / / / / / / /
  / / modFonts.c / /
/ / / / / / / / / /

fallback-aware font ownership

*/

#include "hFiles/modFonts.h"

/* =====
** fonts
** =====
*/

HFONT g_font_ui;
HFONT g_font_big;
HFONT g_font_small;

/* ==============
** fallback faces
** ==============
*/

static const char *g_fallback_faces[] = {
    "Tahoma",
    "Arial",
    "MS Sans Serif",
    "Microsoft Sans Serif",
    0
};

/* ====================
** forward declarations
** ====================
*/

static int CALLBACK FontEnumProc (
    const LOGFONTA    *log_font,
    const TEXTMETRICA *text_metric,
    DWORD              font_type,
    LPARAM             lparam
);

static int FontFaceExists (
    const char *face
);

static HFONT GetDefaultFont (void);

static HFONT TryCreateFontFace (
    int        height,
    int        weight,
    DWORD      quality,
    const char *face
);

/* ============
** create fonts
** ============
*/
void CreateFonts (void)
{
    /* create ui font
    */
    g_font_ui = CreateFontWithFallback (
        -FONT_WINDOW_UI_H,
        FW_BOLD,
        DEFAULT_QUALITY,
        "Comic Sans MS"
    );

    /* create big font
    */
    g_font_big = CreateFontWithFallback (
        -FONT_WINDOW_BIG_H,
        FW_NORMAL,
        DEFAULT_QUALITY,
        "Comic Sans MS"
    );

    /* create small font
    */
    g_font_small = CreateFontWithFallback (
        -FONT_WINDOW_SMALL_H,
        FW_NORMAL,
        DEFAULT_QUALITY,
        "MS Sans Serif"
    );
}

/* ==========
** free fonts
** ==========
*/
void FreeFonts (void)
{
    /* release ui font
    */
    FreeFallbackFont (
        g_font_ui
    );

    /* release big font
    */
    FreeFallbackFont (
        g_font_big
    );

    /* release small font
    */
    FreeFallbackFont (
        g_font_small
    );

    /* clear handles
    */
    g_font_ui = 0;
    g_font_big = 0;
    g_font_small = 0;
}

/* =========================
** create font with fallback
** =========================
*/
HFONT CreateFontWithFallback (
    int        height,
    int        weight,
    DWORD      quality,
    const char *preferred_face)
{
    /* local vars
    */
    HFONT font;
    int   i;

    /* try requested face first
    */
    font = TryCreateFontFace (
        height,
        weight,
        quality,
        preferred_face
    );
    if (font) {
        return font;
    }

    /* try known Windows faces
    */
    for (i = 0; g_fallback_faces[i]; i++) {
        if (
            preferred_face &&
            lstrcmpiA (
                preferred_face,
                g_fallback_faces[i]
            ) == 0) {
            continue;
        }

        font = TryCreateFontFace (
            height,
            weight,
            quality,
            g_fallback_faces[i]
        );
        if (font) {
            return font;
        }
    }

    /* final stock fallback
    */
    return GetDefaultFont();
}

/* ==================
** free fallback font
** ==================
*/
void FreeFallbackFont (
    HFONT font)
{
    /* ignore empty handle
    */
    if (!font) {
        return;
    }

    /* never delete stock font
    */
    if (font == GetDefaultFont()) {
        return;
    }

    /* delete owned font
    */
    DeleteObject (
        font
    );
}

/* ====================
** font enum callback
** ====================
*/
static int CALLBACK FontEnumProc (
    const LOGFONTA    *log_font,
    const TEXTMETRICA *text_metric,
    DWORD              font_type,
    LPARAM             lparam)
{
    /* local vars
    */
    int *found;

    (void)log_font;
    (void)text_metric;
    (void)font_type;

    /* mark face as found
    */
    found = (int *)lparam;
    if (found) {
        *found = TRUE;
    }

    /* stop enumeration
    */
    return 0;
}

/* ================
** font face exists
** ================
*/
static int FontFaceExists (
    const char *face)
{
    /* local vars
    */
    HDC      dc;
    LOGFONTA lf;
    int      found;

    /* reject empty name
    */
    if (
        !face ||
        !face[0]) {
        return FALSE;
    }

    /* get screen dc
    */
    dc = GetDC (
        NULL
    );
    if (!dc) {
        return FALSE;
    }

    /* prepare exact face search
    */
    memset (
        &lf,
        0,
        sizeof(lf)
    );
    lf.lfCharSet = ANSI_CHARSET;
    lstrcpynA (
        lf.lfFaceName,
        face,
        LF_FACESIZE
    );

    /* enumerate matching face
    */
    found = FALSE;
    EnumFontFamiliesExA (
        dc,
        &lf,
        (FONTENUMPROCA)FontEnumProc,
        (LPARAM)&found,
        0
    );

    /* release screen dc
    */
    ReleaseDC (
        NULL,
        dc
    );

    return found;
}

/* ================
** get default font
** ================
*/
static HFONT GetDefaultFont (void)
{
    /* return stock gui font
    */
    return (HFONT)GetStockObject (
        DEFAULT_GUI_FONT
    );
}

/* ====================
** try create font face
** ====================
*/
static HFONT TryCreateFontFace (
    int        height,
    int        weight,
    DWORD      quality,
    const char *face)
{
    /* check installed face
    */
    if (!FontFaceExists (
        face
    )) {
        return 0;
    }

    /* create requested face
    */
    return CreateFontA (
        height,
        0,
        0,
        0,
        weight,
        FALSE,
        FALSE,
        FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        quality,
        DEFAULT_PITCH | FF_DONTCARE,
        face
    );
}
