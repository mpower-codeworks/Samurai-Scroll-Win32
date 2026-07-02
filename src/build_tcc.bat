@echo off
setlocal

echo Building Samurai Scroll Modular-C with Tiny C Compiler...

set OUT_EXE=samurai_scroll.exe
set TCC_COMPAT=tcc_compat
set TCC_MAIN=__tcc_frmMain.c

if not exist "%TCC_COMPAT%\mmsystem.h" (
    echo.
    echo Build failed.
    echo     Missing %TCC_COMPAT%\mmsystem.h
    echo     Extract the tcc_compat folder beside this batch file.
    goto end
)

if not exist frmMain.c (
    echo.
    echo Build failed.
    echo     frmMain.c was not found.
    echo     Run this from the Samurai Scroll project root.
    goto end
)

if exist "%OUT_EXE%" del "%OUT_EXE%" >nul 2>nul
if exist "%TCC_MAIN%" del "%TCC_MAIN%" >nul 2>nul

echo     Preparing TCC-compatible frmMain...

findstr /V /C:"#pragma comment" frmMain.c > "%TCC_MAIN%"

if errorlevel 1 (
    echo.
    echo Build failed.
    echo     Could not create %TCC_MAIN%.
    goto cleanup
)

echo     Compiling...

tcc -Wall -mwindows ^
    -I "%TCC_COMPAT%" ^
    -I "." ^
    -I "modules\hFiles" ^
    -o "%OUT_EXE%" ^
    "%TCC_MAIN%" ^
    modules\modSamuraiAssets.c ^
    modules\modSamuraiAudio.c ^
    modules\modSamuraiGame.c ^
    modules\modFonts.c ^
    modules\modSamuraiPaths.c ^
    modules\modSamuraiRender.c ^
    modules\modSamuraiWindow.c ^
    "%SystemRoot%\System32\user32.dll" ^
    "%SystemRoot%\System32\gdi32.dll" ^
    "%SystemRoot%\System32\winmm.dll"

if errorlevel 1 goto fail

echo.
echo Build complete: %OUT_EXE%
echo Make sure the game exe and the assets folder are
echo next to each other before running the game.
goto cleanup

:fail
echo.
echo Build failed.

:cleanup
if exist "%TCC_MAIN%" del "%TCC_MAIN%" >nul 2>nul

:end
endlocal
