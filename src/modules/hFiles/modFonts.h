/*
    / / / / / / / / / / /
  / / modFonts.h  / /
/ / / / / / / / / / /

fallback-aware font ownership

*/

#ifndef MODFONTS_H
#define MODFONTS_H

#include "modSamuraiCommon.h"

extern HFONT g_font_ui;
extern HFONT g_font_big;
extern HFONT g_font_small;

void CreateFonts (void);
void FreeFonts (void);

HFONT CreateFontWithFallback (
    int        height,
    int        weight,
    DWORD      quality,
    const char *preferred_face
);

void FreeFallbackFont (
    HFONT font
);

#endif
