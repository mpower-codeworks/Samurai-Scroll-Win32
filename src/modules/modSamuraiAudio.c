/*
    / / / / / / / / / / / / /
  / / modSamuraiAudio.c / /
/ / / / / / / / / / / / /

mp3 music with preloading

*/

#include "hFiles/modSamuraiAudio.h"
#include "hFiles/modSamuraiPaths.h"

/* ===========
** music types
** ===========
*/
typedef struct MusicAsset {
    const char *name;
    const char *file;
    const char *alias;
    DWORD       length_ms;
    int         loaded;
} MusicAsset;

enum {
    MUSIC_TITLE,
    MUSIC_GAME,
    MUSIC_WIN,
    MUSIC_LOSE,
    MUSIC_COUNT
};

/* ===========
** music state
** ===========
*/
static MusicAsset g_music[MUSIC_COUNT] = {
    { "title", "kungfu.mp3",   "samurai_title", 0, 0 },
    { "game",  "uyworld.mp3",  "samurai_game",  0, 0 },
    { "win",   "lnbintro.mp3", "samurai_win",   0, 0 },
    { "lose",  "haruumi1.mp3", "samurai_lose",  0, 0 }
};

static int g_music_preloaded;
static int g_music_current = -1;

/* ====================
** forward declarations
** ====================
*/
static int FindMusicAsset (
    const char *file
);
static void BuildMusicPath (
    char       *path,
    const char *file
);
static int OpenMusicAsset (
    MusicAsset *music
);
static void CloseMusicAsset (
    MusicAsset *music
);
static void StopMusicAsset (
    MusicAsset *music
);
static void SeekMusicAssetStart (
    MusicAsset *music
);
static int PlayMusicAsset (
    MusicAsset *music
);
static DWORD QueryMusicLength (
    MusicAsset *music
);
static int MusicRepeatTimerDelay (
    int music_id
);

/* ==========
** load music
** ==========
*/
void LoadMusicAssets (void)
{
    /* preload title music before
    ** the title screen is shown
    */
    g_music_preloaded = PreloadMusicAsset (
        "kungfu.mp3"
    );
}

/* =============
** preload music
** =============
*/
int PreloadMusicAsset (
    const char *file)
{
    /* local vars
    */
    int music_id;

    /* find asset
    */
    music_id = FindMusicAsset(file);
    if (music_id < 0) {
        return 0;
    }

    /* already open
    */
    if (g_music[music_id].loaded) {
        return 1;
    }

    return OpenMusicAsset (
        &g_music[music_id]
    );
}

/* ==========
** free music
** ==========
*/
void FreeMusicAssets (void)
{
    /* local vars
    */
    int i;

    /* stop playback
    */
    StopMusic();

    /* close loaded mp3 aliases
    */
    for (i = 0; i < MUSIC_COUNT; i++) {
        CloseMusicAsset (
            &g_music[i]
        );
    }

    g_music_current = -1;
    g_music_preloaded = 0;
}

/* ==========
** find music
** ==========
*/
static int FindMusicAsset (
    const char *file)
{
    /* local vars
    */
    int i;

    for (i = 0; i < MUSIC_COUNT; i++) {
        if (lstrcmpiA(g_music[i].file, file) == 0) {
            return i;
        }
    }

    return -1;
}

/* ================
** build music path
** ================
*/
static void BuildMusicPath (
    char       *path,
    const char *file)
{
    PathCombineLocal (
        path,
        "assets\\"
    );
    lstrcatA (
        path,
        file
    );
}

/* ==============
** open mp3 asset
** ==============
*/
static int OpenMusicAsset (
    MusicAsset *music)
{
    /* local vars
    */
    char path[MAX_PATH];
    char command[MAX_PATH + 128];
    MCIERROR result;

    /* check state
    */
    if (music == NULL) {
        return 0;
    }
    if (music->loaded) {
        return 1;
    }

    /* build path
    */
    BuildMusicPath (
        path,
        music->file
    );

    /* open as mp3/mpeg media
    */
    lstrcpyA (
        command,
        "open \""
    );
    lstrcatA (
        command,
        path
    );
    lstrcatA (
        command,
        "\" type mpegvideo alias "
    );
    lstrcatA (
        command,
        music->alias
    );

    result = mciSendStringA (
        command,
        NULL,
        0,
        NULL
    );

    /* let MCI infer the device if needed
    */
    if (result != 0) {
        lstrcpyA (
            command,
            "open \""
        );
        lstrcatA (
            command,
            path
        );
        lstrcatA (
            command,
            "\" alias "
        );
        lstrcatA (
            command,
            music->alias
        );

        result = mciSendStringA (
            command,
            NULL,
            0,
            NULL
        );
    }

    if (result != 0) {
        music->loaded = 0;
        music->length_ms = 0;
        return 0;
    }

    /* use milliseconds for status length
    */
    lstrcpyA (
        command,
        "set "
    );
    lstrcatA (
        command,
        music->alias
    );
    lstrcatA (
        command,
        " time format milliseconds"
    );
    mciSendStringA (
        command,
        NULL,
        0,
        NULL
    );

    music->loaded = 1;
    music->length_ms = QueryMusicLength (
        music
    );

    return 1;
}

/* ===============
** close mp3 asset
** ===============
*/
static void CloseMusicAsset (
    MusicAsset *music)
{
    /* local vars
    */
    char command[128];

    if (
        music == NULL ||
        !music->loaded) {
        return;
    }

    lstrcpyA (
        command,
        "close "
    );
    lstrcatA (
        command,
        music->alias
    );
    mciSendStringA (
        command,
        NULL,
        0,
        NULL
    );

    music->loaded = 0;
    music->length_ms = 0;
}

/* ==============
** stop mp3 asset
** ==============
*/
static void StopMusicAsset (
    MusicAsset *music)
{
    /* local vars
    */
    char command[128];

    if (
        music == NULL ||
        !music->loaded) {
        return;
    }

    lstrcpyA (
        command,
        "stop "
    );
    lstrcatA (
        command,
        music->alias
    );
    mciSendStringA (
        command,
        NULL,
        0,
        NULL
    );
}

/* ================
** seek mp3 to top
** ================
*/
static void SeekMusicAssetStart (
    MusicAsset *music)
{
    /* local vars
    */
    char command[128];

    if (
        music == NULL ||
        !music->loaded) {
        return;
    }

    lstrcpyA (
        command,
        "seek "
    );
    lstrcatA (
        command,
        music->alias
    );
    lstrcatA (
        command,
        " to start"
    );
    mciSendStringA (
        command,
        NULL,
        0,
        NULL
    );
}

/* ==============
** play mp3 asset
** ==============
*/
static int PlayMusicAsset (
    MusicAsset *music)
{
    /* local vars
    */
    char command[128];

    if (
        music == NULL ||
        !music->loaded) {
        return 0;
    }

    SeekMusicAssetStart (
        music
    );

    lstrcpyA (
        command,
        "play "
    );
    lstrcatA (
        command,
        music->alias
    );
    lstrcatA (
        command,
        " from 0"
    );

    return mciSendStringA (
        command,
        NULL,
        0,
        NULL) == 0;
}

/* ==================
** query music length
** ==================
*/
static DWORD QueryMusicLength (
    MusicAsset *music)
{
    /* local vars
    */
    char command[128];
    char buffer[64];

    if (
        music == NULL ||
        !music->loaded) {
        return 0;
    }

    ZeroMemory (
        buffer,
        sizeof(buffer)
    );

    lstrcpyA (
        command,
        "status "
    );
    lstrcatA (
        command,
        music->alias
    );
    lstrcatA (
        command,
        " length"
    );

    if (mciSendStringA (
        command,
        buffer,
        sizeof(buffer),
        NULL) != 0) {
        return 0;
    }

    return (DWORD)atol(buffer);
}

/* ==========
** play music
** ==========
*/
void PlayMusic (
    const char *file)
{
    /* local vars
    */
    int music_id;

    /* find asset
    */
    music_id = FindMusicAsset(file);
    if (music_id < 0) {
        return;
    }

    /* already playing
    */
    if (g_music_current == music_id) {
        return;
    }

    /* open if this track was not preloaded
    */
    if (!PreloadMusicAsset(file)) {
        return;
    }

    /* stop previous
    */
    StopMusic();

    /* play track
    */
    if (PlayMusicAsset(&g_music[music_id])) {
        g_music_current = music_id;
        SetTimer (
            g_hwnd,
            TIMER_MUSIC_RESTART,
            (UINT)MusicRepeatTimerDelay(music_id),
            NULL
        );
    }
}

/* ==================
** music repeat delay
** ==================
*/
static int MusicRepeatTimerDelay (
    int music_id)
{
    /* local vars
    */
    DWORD delay;

    if (
        music_id < 0 ||
        music_id >= MUSIC_COUNT) {
        return MUSIC_RESTART_DELAY_MS;
    }

    if (g_music[music_id].length_ms == 0) {
        return 60000 + MUSIC_RESTART_DELAY_MS;
    }

    delay = g_music[music_id].length_ms + MUSIC_RESTART_DELAY_MS;
    if (delay < MUSIC_RESTART_DELAY_MS) {
        delay = MUSIC_RESTART_DELAY_MS;
    }

    return (int)delay;
}

/* =============
** restart music
** =============
*/
void RestartCurrentMusic (void)
{
    /* local vars
    */
    int music_id;

    /* stop restart timer
    */
    KillTimer (
        g_hwnd,
        TIMER_MUSIC_RESTART
    );

    /* capture current track
    */
    music_id = g_music_current;
    if (
        music_id < 0 ||
        music_id >= MUSIC_COUNT) {
        return;
    }

    StopMusicAsset (
        &g_music[music_id]
    );
    if (PlayMusicAsset(&g_music[music_id])) {
        SetTimer (
            g_hwnd,
            TIMER_MUSIC_RESTART,
            (UINT)MusicRepeatTimerDelay(music_id),
            NULL
        );
    }
}

/* ==========
** stop music
** ==========
*/
void StopMusic (void)
{
    /* stop restart timer
    */
    KillTimer (
        g_hwnd,
        TIMER_MUSIC_RESTART
    );

    /* stop current track, but keep
    ** preloaded aliases open
    */
    if (
        g_music_current >= 0 &&
        g_music_current < MUSIC_COUNT) {
        StopMusicAsset (
            &g_music[g_music_current]
        );
        SeekMusicAssetStart (
            &g_music[g_music_current]
        );
    }

    g_music_current = -1;
}
