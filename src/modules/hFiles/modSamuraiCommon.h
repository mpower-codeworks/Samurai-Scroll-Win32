/*
    / / / / / / / / / / / / / /
  / / modSamuraiCommon.h  / /
/ / / / / / / / / / / / / /

shared constants and common Win32 declarations

*/

#ifndef MODSAMURAICOMMON_H
#define MODSAMURAICOMMON_H

#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* =================
** program constants
** =================
*/

#define APP_NAME        "Samurai Scroll - Win32/GDI"
#define LOGICAL_W       504
#define LOGICAL_H       399
#define TILE            32
#define VIEW_TILES      7
#define MAP_H           36
#define MAP_W           46
#define MAX_LOG_LINES   128
#define MAX_LOG_LEN     256

#define TIMER_CLOCK          1
#define TIMER_OUTTRO         2
#define TIMER_OUTTRO_START   3
#define TIMER_MUSIC_RESTART  4

#define FONT_WINDOW_UI_H              15
#define FONT_WINDOW_BIG_H             22
#define FONT_WINDOW_SMALL_H           12

#define FONT_FULLSCREEN_TEXT_DIV      46
#define FONT_FULLSCREEN_TEXT_MIN      16
#define FONT_FULLSCREEN_TEXT_MAX      28

#define FONT_FULLSCREEN_SMALL_DIV     62
#define FONT_FULLSCREEN_SMALL_MIN     12
#define FONT_FULLSCREEN_SMALL_MAX     20

#define FONT_FULLSCREEN_TIMER_DIV     10
#define FONT_FULLSCREEN_TIMER_MIN     52
#define FONT_FULLSCREEN_TIMER_MAX     120

#define FONT_FULLSCREEN_EQUIP_DIV     30
#define FONT_FULLSCREEN_EQUIP_MIN     30
#define FONT_FULLSCREEN_EQUIP_MAX     58

#define FONT_FULLSCREEN_TITLE_DIV     34
#define FONT_FULLSCREEN_TITLE_MIN     18
#define FONT_FULLSCREEN_TITLE_MAX     34

#define START_IN_FULLSCREEN           1
#define OUTTRO_START_DELAY_MS         3000
#define OUTTRO_MOVE_MS                384
#define OUTTRO_MOVE_JITTER_MS         90
#define OUTTRO_PAUSE_MIN_MS           650
#define OUTTRO_PAUSE_MAX_MS           1800
#define OUTTRO_STEPS_MIN              3
#define OUTTRO_STEPS_MAX              8
#define MUSIC_RESTART_DELAY_MS        5000

/* ============
** asset types
** ============
*/

typedef struct BitmapAsset {
    const char *name;
    const char *file;
    HBITMAP     bmp;
    HBITMAP     old_bmp;
    HDC         dc;
    int         w;
    int         h;
} BitmapAsset;

enum {
    BMP_BB, BMP_BL, BMP_BR, BMP_BRIDGE, BMP_BUSH,
    BMP_FOREST, BMP_GRASS, BMP_GUY1, BMP_GUY2,
        BMP_GUY3, BMP_GUY4, BMP_GUY5,
    BMP_ILB, BMP_ILT, BMP_IRB, BMP_IRT,
    BMP_L, BMP_R,
    BMP_MOUNT1, BMP_MOUNT2, BMP_MOUNT3,
        BMP_MOUNT4, BMP_MOUNTAIN,
    BMP_SCROLL, BMP_SIGN, BMP_SKULL,
    BMP_TL, BMP_TORI, BMP_TR, BMP_TT,
    BMP_VILLAGE, BMP_BLACK,
    BMP_DOWNG, BMP_UPG, BMP_LEFTG, BMP_RIGHTG,
    BMP_TITLE,
    BMP_COUNT
};

/* ================
** frmMain globals
** ================
*/

extern HWND g_hwnd;

#endif
