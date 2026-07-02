/*
    / / / / / / / / / / / / / /
  / / modSamuraiAssets.c  / /
/ / / / / / / / / / / / / /

bitmap and buffer ownership

*/

#include "hFiles/modSamuraiAssets.h"
#include "hFiles/modSamuraiPaths.h"

/* ==========
** bitmaps
** ==========
*/

BitmapAsset g_bmp[BMP_COUNT] = {
    { "bb",       "assets\\bb.bmp",       0 },
    { "bl",       "assets\\bl.bmp",       0 },
    { "br",       "assets\\br.bmp",       0 },
    { "bridge",   "assets\\bridge.bmp",   0 },
    { "bush",     "assets\\bush.bmp",     0 },
    { "forest",   "assets\\forest.bmp",   0 },
    { "grass",    "assets\\grass.bmp",    0 },
    { "guy1",     "assets\\guy1.bmp",     0 },
    { "guy2",     "assets\\guy2.bmp",     0 },
    { "guy3",     "assets\\guy3.bmp",     0 },
    { "guy4",     "assets\\guy4.bmp",     0 },
    { "guy5",     "assets\\guy5.bmp",     0 },
    { "ilb",      "assets\\ilb.bmp",      0 },
    { "ilt",      "assets\\ilt.bmp",      0 },
    { "irb",      "assets\\irb.bmp",      0 },
    { "irt",      "assets\\irt.bmp",      0 },
    { "l",        "assets\\l.bmp",        0 },
    { "r",        "assets\\r.bmp",        0 },
    { "mount1",   "assets\\mount1.bmp",   0 },
    { "mount2",   "assets\\mount2.bmp",   0 },
    { "mount3",   "assets\\mount3.bmp",   0 },
    { "mount4",   "assets\\mount4.bmp",   0 },
    { "mountain", "assets\\mountain.bmp", 0 },
    { "scroll",   "assets\\scroll.bmp",   0 },
    { "sign",     "assets\\sign.bmp",     0 },
    { "skull",    "assets\\skull.bmp",    0 },
    { "tl",       "assets\\tl.bmp",       0 },
    { "tori",     "assets\\tori.bmp",     0 },
    { "tr",       "assets\\tr.bmp",       0 },
    { "tt",       "assets\\tt.bmp",       0 },
    { "village",  "assets\\village.bmp",  0 },
    { "black",    "assets\\black.bmp",    0 },
    { "downg",    "assets\\downg.bmp",    0 },
    { "upg",      "assets\\upg.bmp",      0 },
    { "leftg",    "assets\\leftg.bmp",    0 },
    { "rightg",   "assets\\rightg.bmp",   0 },
    { "title",    "assets\\title.bmp",    0 }
};

/* ==============
** buffer handles
** ==============
*/

HDC     g_back_dc;
HBITMAP g_back_bmp;
HBITMAP g_back_old;

HDC     g_present_dc;
HBITMAP g_present_bmp;
HBITMAP g_present_old;
int     g_present_w;
int     g_present_h;

/* ============
** load bitmaps
** ============
*/
void LoadAssets (void)
{
    /* local vars
    */
    int i;
    HDC screen_dc;

    /* get screen dc
    */
    screen_dc = GetDC(g_hwnd);

    /* load each bitmap
    */
    for (i = 0; i < BMP_COUNT; i++) {
        char path[MAX_PATH];
        BITMAP bm;

        PathCombineLocal (
            path,
            g_bmp[i].file
        );
        g_bmp[i].bmp = (HBITMAP)LoadImageA (
            NULL,
            path,
            IMAGE_BITMAP,
            0,
            0,
            LR_LOADFROMFILE |
            LR_CREATEDIBSECTION
        );

        if (g_bmp[i].bmp) {
            GetObject (
                g_bmp[i].bmp,
                sizeof(bm),
                &bm
            );
            g_bmp[i].w = bm.bmWidth;
            g_bmp[i].h = bm.bmHeight;

            /* make asset dc
            */
            g_bmp[i].dc = CreateCompatibleDC (
                screen_dc
            );
            
            if (g_bmp[i].dc) {
                g_bmp[i].old_bmp = (HBITMAP)SelectObject (
                    g_bmp[i].dc,
                    g_bmp[i].bmp
                );
            }
        }
    }

    /* release screen dc
    */
    ReleaseDC (
        g_hwnd,
        screen_dc
    );
}

/* ============
** free bitmaps
** ============
*/
void FreeAssets (void)
{
    /* local vars
    */
    int i;

    /* release each bitmap
    */
    for (i = 0; i < BMP_COUNT; i++) {
        if (g_bmp[i].dc) {
            SelectObject (
                g_bmp[i].dc,
                g_bmp[i].old_bmp
            );
            DeleteDC(g_bmp[i].dc);
            g_bmp[i].dc = 0;
            g_bmp[i].old_bmp = 0;
        }
        if (g_bmp[i].bmp) {
            DeleteObject(g_bmp[i].bmp);
            g_bmp[i].bmp = 0;
        }
        g_bmp[i].w = 0;
        g_bmp[i].h = 0;
    }
}

/* ================
** make back buffer
** ================
*/
void MakeBackBuffer (
    HWND hwnd)
{
    /* local vars
    */
    HDC dc;

    /* create compatible dc
    */
    dc = GetDC(hwnd);
    g_back_dc = CreateCompatibleDC(dc);
    g_back_bmp = CreateCompatibleBitmap (
        dc,
        LOGICAL_W,
        LOGICAL_H
    );
    g_back_old = (HBITMAP)SelectObject (
        g_back_dc,
        g_back_bmp
    );
    ReleaseDC (
        hwnd,
        dc
    );
}

/* ================
** free back buffer
** ================
*/
void FreeBackBuffer (void)
{
    /* check buffer
    */
    if (g_back_dc) {
        SelectObject (
            g_back_dc,
            g_back_old
        );
        DeleteObject(g_back_bmp);
        DeleteDC(g_back_dc);
        g_back_dc = 0;
        g_back_bmp = 0;
        g_back_old = 0;
    }
}

/* =====================
** ensure present buffer
** =====================
*/
int EnsurePresentBuffer (
    HDC hdc,
    int w,
    int h)
{
    /* local vars
    */
    HBITMAP new_bmp;

    /* validate size
    */
    if (w <= 0 || h <= 0) {
        return 0;
    }

    /* reuse buffer
    */
    if (
        g_present_dc     &&
        g_present_bmp    &&
        g_present_w == w &&
        g_present_h == h) {
        return 1;
    }

    /* rebuild buffer
    */
    FreePresentBuffer();

    g_present_dc = CreateCompatibleDC(hdc);
    if (!g_present_dc) {
        return 0;
    }

    new_bmp = CreateCompatibleBitmap (
        hdc,
        w,
        h
    );
    if (!new_bmp) {
        DeleteDC(g_present_dc);
        g_present_dc = 0;
        return 0;
    }

    g_present_bmp = new_bmp;
    g_present_old = (HBITMAP)SelectObject (
        g_present_dc,
        g_present_bmp
    );
    g_present_w = w;
    g_present_h = h;
    return 1;
}

/* ===================
** free present buffer
** ===================
*/
void FreePresentBuffer (void)
{
    /* check buffer
    */
    if (g_present_dc) {
        SelectObject (
            g_present_dc,
            g_present_old
        );
        if (g_present_bmp) {
            DeleteObject (
                g_present_bmp
            );
        }
        
        DeleteDC(g_present_dc);
        
        g_present_dc  = 0;
        g_present_bmp = 0;
        g_present_old = 0;
        g_present_w   = 0;
        g_present_h   = 0;
    }
}
