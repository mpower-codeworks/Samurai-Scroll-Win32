# Samurai-Scroll-Win32
<img src="Images/logo.jpg" align="left" width="100" alt="Samurai Logo">

Samurai Scroll Win32 is a C port of a small tile-based RPG from 1999. It was originally written in VB6 by Jesse Acosta. The idea is to preserve this little gem before it disappears from the Internet. 

This version retains the graphics and gameplay while adding full-screen and some fun improvements that fit within the spirit of the original. You can use F11 to toggle to windowed-mode, which makes it identical to Jesse's original.

While indeed a short game, Samurai Scroll has charm and replayability.

<table>
    <tr>
        <td align="left" width="40%" valign="top">
            <img src="Images/title.jpg" width="100%" alt="title_screen"><br>
        </td>
        <td align="left" width="50%" valign="center">
            <img src="Images/game-full.jpg" width="100%" alt="royal_bed"><br>
        </td>
    </tr>
</table>
It is my hope that Samurai Scroll will continue to inspire those interested in making a small tile-based game, as it has done for me. Some fun experiments might be changing the graphics, changing the music, changing the title screen -- all easily done without a single line of code.

If you'd like to modify the code and rebuild, you can use Visual Studio or Tiny C Compiler. The differences in resulting exe sizes might be of interest. Once installed, from command prompt run build_msvc.bat or build_tcc.bat.

You can modify the map in the file modSamuraiGame.c - good luck!
