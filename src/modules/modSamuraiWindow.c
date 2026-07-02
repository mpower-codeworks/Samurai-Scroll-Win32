/*
    / / / / / / / / / / / / / /
  / / modSamuraiWindow.c  / /
/ / / / / / / / / / / / / /

main window fullscreen behavior

*/

#include "hFiles/modSamuraiWindow.h"

/* ===========
** module vars
** ===========
*/
int g_fullscreen;
static DWORD g_saved_style;
static DWORD g_saved_exstyle;
static WINDOWPLACEMENT g_saved_wp =
    { sizeof(WINDOWPLACEMENT) };

/* =================
** toggle fullscreen
** =================
*/
void ToggleFullscreen (
    HWND hwnd)
{
    /* local vars
    */
    /* enter or leave
    */
    if (!g_fullscreen) {
        MONITORINFO mi;

        /* save window state
        */
        g_saved_style = (DWORD)GetWindowLongPtr (
            hwnd,
            GWL_STYLE
        );
        g_saved_exstyle = (DWORD)GetWindowLongPtr (
            hwnd,
            GWL_EXSTYLE
        );
        g_saved_wp.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement (
            hwnd,
            &g_saved_wp
        );

        mi.cbSize = sizeof(mi);
        GetMonitorInfoA (
            MonitorFromWindow (
                hwnd,
                MONITOR_DEFAULTTONEAREST),
                &mi
        );

        /* apply popup frame
        */
        SetWindowLongPtr (
            hwnd,
            GWL_STYLE,
            WS_POPUP | WS_VISIBLE
        );
        SetWindowLongPtr (
            hwnd,
            GWL_EXSTYLE,
            0
        );
        SetWindowPos (
            hwnd,
            HWND_TOP,
            mi.rcMonitor.left,
            mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top,
            SWP_FRAMECHANGED | SWP_SHOWWINDOW
        );
        g_fullscreen = 1;
        SetCursor(NULL);
    } else {
        /* restore frame
        */
        SetWindowLongPtr (
            hwnd,
            GWL_STYLE,
            g_saved_style
        );
        SetWindowLongPtr (
            hwnd,
            GWL_EXSTYLE,
            g_saved_exstyle
        );
        SetWindowPlacement (
            hwnd,
            &g_saved_wp
        );
        SetWindowPos (
            hwnd,
            NULL,
            0,
            0,
            0,
            0,
            SWP_NOMOVE |
            SWP_NOSIZE |
            SWP_NOZORDER |
            SWP_NOOWNERZORDER |
            SWP_FRAMECHANGED |
            SWP_SHOWWINDOW
        );
        g_fullscreen = 0;
        SetCursor (
            LoadCursor (
                NULL,
                IDC_ARROW
            )
        );
    }

    InvalidateRect (
        hwnd,
        NULL,
        FALSE
    );
}
