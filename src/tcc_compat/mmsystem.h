/*
    / / / / / / / / / / / /
  / / mmsystem.h TCC  / /
/ / / / / / / / / / / /

minimal WinMM declarations for Samurai Scroll TCC builds

this file is only used when Tiny C Compiler does not include
mmsystem.h. the real functions still link from winmm.dll

*/

#ifndef SAMURAI_TCC_MMSYSTEM_H
#define SAMURAI_TCC_MMSYSTEM_H

#include <windows.h>

/* =================
** missing base types
** =================
*/
#ifndef DWORD_PTR
#define DWORD_PTR DWORD
#endif

/* ====================
** multimedia constants
** ====================
*/
#ifndef CALLBACK_NULL
#define CALLBACK_NULL        0x00000000
#endif

#ifndef MMSYSERR_NOERROR
#define MMSYSERR_NOERROR     0
#endif

#ifndef WAVE_MAPPER
#define WAVE_MAPPER          ((UINT)-1)
#endif

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM      1
#endif

#ifndef MM_WOM_DONE
#define MM_WOM_DONE          0x03BD
#endif

#ifndef SND_SYNC
#define SND_SYNC             0x0000
#endif

#ifndef SND_ASYNC
#define SND_ASYNC            0x0001
#endif

#ifndef SND_NODEFAULT
#define SND_NODEFAULT        0x0002
#endif

#ifndef SND_MEMORY
#define SND_MEMORY           0x0004
#endif

#ifndef SND_LOOP
#define SND_LOOP             0x0008
#endif

#ifndef SND_NOSTOP
#define SND_NOSTOP           0x0010
#endif

#ifndef SND_PURGE
#define SND_PURGE            0x0040
#endif

#ifndef SND_APPLICATION
#define SND_APPLICATION      0x0080
#endif

#ifndef SND_FILENAME
#define SND_FILENAME         0x00020000
#endif

#ifndef SND_RESOURCE
#define SND_RESOURCE         0x00040004
#endif

/* ==========
** wave types
** ==========
*/
#ifndef SAMURAI_TCC_WAVE_TYPES
#define SAMURAI_TCC_WAVE_TYPES

typedef UINT MMRESULT;
typedef DWORD MCIERROR;

typedef struct HWAVEOUT__ *HWAVEOUT;

typedef struct waveformat_tag {
    WORD  wFormatTag;
    WORD  nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD  nBlockAlign;
    WORD  wBitsPerSample;
    WORD  cbSize;
} WAVEFORMATEX;

typedef struct wavehdr_tag {
    LPSTR     lpData;
    DWORD     dwBufferLength;
    DWORD     dwBytesRecorded;
    DWORD_PTR dwUser;
    DWORD     dwFlags;
    DWORD     dwLoops;
    struct wavehdr_tag *lpNext;
    DWORD_PTR reserved;
} WAVEHDR;

#endif

/* =================
** WinMM prototypes
** =================
*/
MMRESULT WINAPI waveOutOpen (
    HWAVEOUT     *phwo,
    UINT          uDeviceID,
    WAVEFORMATEX *pwfx,
    DWORD_PTR     dwCallback,
    DWORD_PTR     dwInstance,
    DWORD         fdwOpen
);

MMRESULT WINAPI waveOutPrepareHeader (
    HWAVEOUT hwo,
    WAVEHDR *pwh,
    UINT     cbwh
);

MMRESULT WINAPI waveOutWrite (
    HWAVEOUT hwo,
    WAVEHDR *pwh,
    UINT     cbwh
);

MMRESULT WINAPI waveOutReset (
    HWAVEOUT hwo
);

MMRESULT WINAPI waveOutUnprepareHeader (
    HWAVEOUT hwo,
    WAVEHDR *pwh,
    UINT     cbwh
);

MMRESULT WINAPI waveOutClose (
    HWAVEOUT hwo
);

BOOL WINAPI PlaySoundA (
    LPCSTR  pszSound,
    HMODULE hmod,
    DWORD   fdwSound
);

MCIERROR WINAPI mciSendStringA (
    LPCSTR lpszCommand,
    LPSTR  lpszReturnString,
    UINT   cchReturn,
    HWND   hwndCallback
);

#endif
