@echo off
setlocal

echo Building Samurai Scroll Modular-C with MSVC...

cl /nologo /O2 /W3 /I modules\hFiles /Fe:samurai_scroll.exe ^
    frmMain.c ^
    modules\modSamuraiAssets.c ^
    modules\modSamuraiAudio.c ^
    modules\modSamuraiGame.c ^
    modules\modFonts.c ^
    modules\modSamuraiPaths.c ^
    modules\modSamuraiRender.c ^
    modules\modSamuraiWindow.c ^
    /link /SUBSYSTEM:WINDOWS user32.lib gdi32.lib winmm.lib
if errorlevel 1 goto fail

echo.
echo Build complete: samurai_scroll.exe
echo Make sure the game exe and the assets folder are
echo next to each other before running the game.
goto end

:fail
echo.
echo Build failed.

:end
del *.obj 2>nul
del modules\*.obj 2>nul
endlocal
