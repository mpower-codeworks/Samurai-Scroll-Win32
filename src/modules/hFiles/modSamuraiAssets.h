/*
    / / / / / / / / / / / / / /
  / / modSamuraiAssets.h  / /
/ / / / / / / / / / / / / /

bitmap and buffer ownership

*/

#ifndef MODSAMURAIASSETS_H
#define MODSAMURAIASSETS_H

#include "modSamuraiCommon.h"

extern BitmapAsset g_bmp[BMP_COUNT];

extern HDC     g_back_dc;
extern HBITMAP g_back_bmp;
extern HBITMAP g_back_old;

extern HDC     g_present_dc;
extern HBITMAP g_present_bmp;
extern HBITMAP g_present_old;
extern int     g_present_w;
extern int     g_present_h;

void LoadAssets (void);
void FreeAssets (void);

void MakeBackBuffer (
    HWND hwnd
);

void FreeBackBuffer (void);

int EnsurePresentBuffer (
    HDC hdc,
    int w,
    int h
);

void FreePresentBuffer (void);

#endif
