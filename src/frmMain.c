/*
    / / / / / / / / /
  / / frmMain.c / /
/ / / / / / / / /

Samurai Scroll main window and message dispatch

*/

#include "modules/hFiles/modSamuraiCommon.h"
#include "modules/hFiles/modSamuraiAssets.h"
#include "modules/hFiles/modSamuraiAudio.h"
#include "modules/hFiles/modFonts.h"
#include "modules/hFiles/modSamuraiGame.h"
#include "modules/hFiles/modSamuraiPaths.h"
#include "modules/hFiles/modSamuraiRender.h"
#include "modules/hFiles/modSamuraiWindow.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winmm.lib")

/* ==============
** window globals
** ==============
*/
HWND g_hwnd;
static HINSTANCE g_inst;

#define WM_PRELOAD_GAME_MUSIC (WM_APP + 1)

/* ====================
** forward declarations
** ====================
*/
static LRESULT CALLBACK WndProc (
    HWND   hwnd,
    UINT   msg,
    WPARAM wparam,
    LPARAM lparam
);

/* ===========
** entry point
** ===========
*/
int WINAPI WinMain (
    HINSTANCE hInstance,
    HINSTANCE hPrev,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    /* local vars
    */
    WNDCLASSA wc;
    HWND      hwnd;
    RECT      rc;
    int       win_w;
    int       win_h;

    (void)hPrev;
    (void)lpCmdLine;

    /* save instance
    */
    g_inst = hInstance;
    srand (
        (unsigned int)time(NULL)
    );
    
    GetExeDir();

    /* register class
    */
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.hInstance   = hInstance;
    
    wc.hCursor = LoadCursor (
        NULL,
        IDC_ARROW
    );
    
    wc.hIcon = LoadIcon (
        NULL,
        IDI_APPLICATION
    );
    
    wc.hbrBackground = NULL;
    
    wc.lpszClassName =
        "SamuraiScrollWin32Class";

    if (!RegisterClassA(&wc)) {
        MessageBoxA (
            NULL,
            "Could not register window class.",
            APP_NAME,
            MB_ICONERROR
        );
        return 0;
    }

    /* calculate window
    */
    win_w = LOGICAL_W * 2;
    win_h = LOGICAL_H * 2;
    rc.left = 0;
    rc.top = 0;
    rc.right = win_w;
    rc.bottom = win_h;
    
    AdjustWindowRect (
        &rc,
        WS_OVERLAPPEDWINDOW,
        FALSE
    );

    /* create window
    */
    hwnd = CreateWindowA(
        "SamuraiScrollWin32Class",
        APP_NAME,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rc.right - rc.left,
        rc.bottom - rc.top,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hwnd) {
        MessageBoxA (
            NULL,
            "Could not create window.",
            APP_NAME, MB_ICONERROR
        );
        return 0;
    }

    /* save window
    */
    g_hwnd = hwnd;

    /* load every external asset before
    ** the title screen is shown
    */
    LoadAssets();
    LoadMusicAssets();

    /* show window
    */
    ShowWindow(hwnd, nCmdShow);
    if (START_IN_FULLSCREEN) {
        ToggleFullscreen(hwnd);
    }
    UpdateWindow(hwnd);
    PlayMusic("kungfu.mp3");

    /* queue game music preload after the
    ** title screen has started
    */
    PostMessageA (
        hwnd,
        WM_PRELOAD_GAME_MUSIC,
        0,
        0
    );

    /* message loop
    */
    for (;;) {
        MSG msg;
        int result;

        result = GetMessageA (
            &msg,
            NULL,
            0,
            0
        );
        if (result <= 0) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    /* cleanup
    */
    StopMusic();
    FreeMusicAssets();
    FreeAssets();
    FreeFonts();
    FreePresentBuffer();
    FreeBackBuffer();

    return 0;
}

/* ================
** window procedure
** ================
*/
static LRESULT CALLBACK WndProc (
    HWND   hwnd,
    UINT   msg,
    WPARAM wparam,
    LPARAM lparam)
{
    /* local vars
    */
    /* dispatch message
    */
    switch (msg) {
    case WM_CREATE:
        /* create drawing resources
        */
        MakeBackBuffer(hwnd);
        CreateFonts();
        return 0;

    case WM_SIZE:
        InvalidateRect (
            hwnd,
            NULL,
            FALSE
        );
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
        /* paint frame
        */
        {
            PAINTSTRUCT ps;
            HDC hdc;

            hdc = BeginPaint (
                hwnd,
                &ps
            );
            
            Render (
                hwnd,
                hdc
            );
            
            EndPaint (
                hwnd,
                &ps
            );
        }
        return 0;

    case WM_SETCURSOR:
        if (
            g_fullscreen &&
            LOWORD(lparam) == HTCLIENT) {
            SetCursor(NULL);
            return TRUE;
        }
        break;

    case WM_LBUTTONDOWN:
        if (g_on_title) {
            InitGame();
        }
        return 0;

    case WM_PRELOAD_GAME_MUSIC:
        /* preload game music after the title
        ** screen is visible and playing
        */
        PreloadMusicAsset (
            "uyworld.mp3"
        );
        return 0;

    case WM_TIMER:
        /* handle timers
        */
        if (
            wparam == TIMER_CLOCK &&
            !g_on_title && g_timer_on) {
            g_time_left--;
            if (g_time_left < 0) {
                LoseGame();
            }
            InvalidateRect (
                hwnd,
                NULL,
                FALSE
            );
        } else if (
            wparam == TIMER_OUTTRO_START &&
            g_outtro_on) {
            BeginWinOuttroWalk();
        } else if (
            wparam == TIMER_OUTTRO &&
            g_outtro_on) {
            OuttroStep();
        } else if (
            wparam == TIMER_MUSIC_RESTART) {
            RestartCurrentMusic();
        }
        return 0;

    case WM_CHAR:
        /* handle typed keys
        */
        if (g_on_title) {
            if (
                wparam == VK_RETURN ||
                wparam == ' ') {
                InitGame();
            }
            return 0;
        }

        if (
            g_outtro_on ||
            g_lost) {
            return 0;
        }

        switch (wparam) {
        case '8':
            MovePlayer (
                0,
               -1,
                2
            );
            break;
        case '2':
            MovePlayer (
                0,
                1,
                1
            );
            break;
        case '4':
            MovePlayer (
               -1,
                0,
                3
            );
            break;
        case '6':
            MovePlayer (
                1,
                0,
                4
            );
            break;
        case 't':
        case 'T':
            TalkToOthers();
            InvalidateRect (
                hwnd,
                NULL,
                FALSE
            );
            break;
        }
        return 0;

    case WM_KEYDOWN:
        /* handle virtual keys
        */
        if (
            g_on_title &&
            wparam == VK_ESCAPE) {
            DestroyWindow(hwnd);
            return 0;
        }

        /* return to title
        */
        if (
            !g_on_title &&
            wparam == VK_ESCAPE) {
            ReturnToTitle();
            return 0;
        }

        /* block ended screens
        */
        if (
            g_outtro_on ||
            g_lost) {
            return 0;
        }

        /* toggle fullscreen
        */
        if (wparam == VK_F11) {
            ToggleFullscreen(hwnd);
            return 0;
        }

        if (!g_on_title) {
            if (wparam == VK_UP) {
                MovePlayer (
                    0,
                   -1,
                    2
                );
                return 0;
            }
            if (wparam == VK_DOWN) {
                MovePlayer (
                    0,
                    1,
                    1
                );
                return 0;
            }
            if (wparam == VK_LEFT) {
                MovePlayer (
                   -1,
                    0,
                    3
                );
                return 0;
            }
            if (wparam == VK_RIGHT) {
                MovePlayer (
                    1,
                    0,
                    4
                );
                return 0;
            }
        }
        return 0;

    case MM_WOM_DONE:
        /* WAV restart timing handled
        ** by TIMER_MUSIC_RESTART
        */
        return 0;

    case WM_DESTROY:
        /* shutdown timers
        */
        StopMusic();
        KillTimer (
            hwnd,
            TIMER_CLOCK
        );
        KillTimer (
            hwnd,
            TIMER_OUTTRO_START
        );
        KillTimer (
            hwnd,
            TIMER_OUTTRO
        );
        KillTimer (
            hwnd,
            TIMER_MUSIC_RESTART
        );
        FreeFonts();
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA (
        hwnd,
        msg,
        wparam,
        lparam
    );
}
