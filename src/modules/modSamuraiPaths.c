/*
    / / / / / / / / / / / / /
  / / modSamuraiPaths.c / /
/ / / / / / / / / / / / /

local executable path helpers

*/

#include "hFiles/modSamuraiPaths.h"

/* ===========
** module vars
** ===========
*/

static char g_exe_dir[MAX_PATH];

/* ==============
** get exe folder
** ==============
*/
void GetExeDir (void)
{
    /* local vars
    */
    DWORD len;
    char *p;

    len = GetModuleFileNameA (
        NULL,
        g_exe_dir,
        MAX_PATH
    );
    if (len == 0 || len >= MAX_PATH) {
        lstrcpyA (
            g_exe_dir,
            "."
        );
        return;
    }

    p = g_exe_dir + lstrlenA(g_exe_dir);
    while (p > g_exe_dir) {
        if (*p == '\\' || *p == '/') {
            *p = 0;
            return;
        }
        p--;
    }

    lstrcpyA (
        g_exe_dir,
        "."
    );
}

/* ==================
** combine local path
** ==================
*/
void PathCombineLocal (
    char       *out,
    const char *rel)
{
    /* local vars
    */
    lstrcpyA (
        out,
        g_exe_dir
    );
    lstrcatA (
        out,
        "\\"
    );
    lstrcatA (
        out,
        rel
    );
}
