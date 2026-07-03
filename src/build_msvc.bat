@echo off
setlocal

echo Building Samurai Scroll Modular-C with MSVC...

if not exist assets\icon.ico goto missing_icon

del samurai_scroll.res 2>nul

rc /nologo /fo samurai_scroll.res samurai_scroll.rc
if errorlevel 1 goto fail

cl /nologo /O2 /W3 /I modules\hFiles /Fe:samurai_scroll.exe ^
    frmMain.c ^
    modules\modSamuraiAssets.c ^
    modules\modSamuraiAudio.c ^
    modules\modSamuraiGame.c ^
    modules\modFonts.c ^
    modules\modSamuraiPaths.c ^
    modules\modSamuraiRender.c ^
    modules\modSamuraiWindow.c ^
    samurai_scroll.res ^
    /link /SUBSYSTEM:WINDOWS user32.lib gdi32.lib winmm.lib
if errorlevel 1 goto fail

echo.
echo Build complete: samurai_scroll.exe
echo Make sure the game exe and the assets folder are
echo next to each other before running the game.
goto end

:missing_icon
echo.
echo Missing icon file: assets\icon.ico
goto fail

:fail
echo.
echo Build failed.

:end
del *.obj 2>nul
del modules\*.obj 2>nul
del samurai_scroll.res 2>nul
endlocal
