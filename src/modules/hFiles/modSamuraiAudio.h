/*
    / / / / / / / / / / / / /
  / / modSamuraiAudio.h / /
/ / / / / / / / / / / / /

mp3 music playback with selected preloading

*/

#ifndef MODSAMURAIAUDIO_H
#define MODSAMURAIAUDIO_H

#include "modSamuraiCommon.h"

void LoadMusicAssets (void);
void FreeMusicAssets (void);

int PreloadMusicAsset (
    const char *file
);

void PlayMusic (
    const char *file
);

void RestartCurrentMusic (void);
void StopMusic (void);

#endif
