/*
    / / / / / / / / / / / / /
  / / modSamuraiGame.c  / /
/ / / / / / / / / / / / /

map movement timer and story logic

*/

#include "hFiles/modSamuraiGame.h"
#include "hFiles/modSamuraiAudio.h"

/* ==============
** game constants
** ==============
*/

#define GAMEPLAY_HELP_MOVE_LIMIT 5

/* ===========
** game state
** ===========
*/

int g_on_title                 = 1;
int g_timer_on                 = 0;
int g_won                      = 0;
int g_lost                     = 0;
int g_outtro_on                = 0;
int g_outtro_last_dx           = 0;
int g_outtro_last_dy           = 0;
int g_outtro_steps_until_pause = 0;
int g_found_scroll             = 0;
int g_talked_to_sensei         = 0;
int g_time_left                = 100;
int g_char_x                   = 5;
int g_char_y                   = 5;

/* 1 down, 2 up, 3 left, 4 right
*/
int g_char_face                = 1;

char g_map[MAP_H][MAP_W + 1];
char g_log[MAX_LOG_LINES][MAX_LOG_LEN];
int  g_log_count;

static int g_gameplay_help_moves;
static int g_gameplay_help_visible;

/* =========================
** original map and sayings
** =========================
*/
static const char *g_original_map[MAP_H] = {
    "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV",
    "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV",
    "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV",
    "VVVQTTTTTTTTTTTTTTTTTTTTTTTTTTTTSVVVVVVVVVVVVV",
    "VVVG55555555555555555566B6646666MVVQTTTTTSVVVV",
    "VVVG55U5566665555556666666666666MVVG66666MVVVV",
    "VVVG5566P66666655566666466666666MVVG66666MVVVV",
    "VVVG6666664666646666666666666666MVV1003002VVVV",
    "VVVG66666666666666666666E0C66466MVVVVV3VVVVVVV",
    "VVVG55666666666666646666MVG66666FTTTTT3TTTSVVV",
    "VVVG55555555555555666L66FTD66666L66666666PMVVV",
    "VVVG555555556646666666666666a6666646666466MVVV",
    "VVVG55555566666664666466646666646666666666MVVV",
    "VVVG66555666666666666666666666666555564666MVVV",
    "VVVG666466646664666666666666666L5555555546MVVV",
    "VVVG666666666L666666666666P664666665555666MVVV",
    "VVVG6466664666666666L666666666666665556966MVVV",
    "VVVG6666666666666L666666666666466555566666MVVV",
    "VVVG66646666664666646666666646666555666666MVVV",
    "VVVG6666666666666666666L666666655566666646MVVV",
    "VVVG664L6666666666666666666666555666466666MVVV",
    "VVVG6666444444444464466666L655555556664666MVVV",
    "VVVG676646O6666664644666666666655555556666MVVV",
    "VVVG666646664444466646666666666666666E00002VVV",
    "VVVG6666446666466646464666666466L6666FTTTTSVVV",
    "VVVG66664664444446464646666666666666666466MVVV",
    "VVVG66664666666666644466646666666666646666MVVV",
    "VVVG66664444444444644666666646686666666666MVVV",
    "VVVG66L6666HI666666666666666666666L6666646MVVV",
    "VVVG66666LLJKLL666666P66666666646666466666MVVV",
    "VVVG666LLLLL66LL66666666646666666666664666MVVV",
    "VVVG66666666666666666666666666666666666666MVVV",
    "VVV1000000000000000000000000000000000000002VVV",
    "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV",
    "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV",
    "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV"
};

static const char *g_answers[10] = {
  "Whoever has walked with truth generates life.",
  "Do not cut off the neck of that which has had its neck cut off.",
  "That which is given in submission becomes a medium of defiance.",
  "The destruction is from his own personal god; he knows no savior.",
  "Wealth is hard to come by, but poverty is always at hand.",
  "He acquires many things, he must keep close watch over them.",
  "Have you tried the Southwest reigon of Booken Island?",
  "The Northeast reigon of Booken Island might be promising.",
  "I have heard that the scroll has been seen in the Northwest.",
  "The elders believe the scroll lies in the Southeast"
};

/* ====================
** forward declarations
** ====================
*/
static void InitMap (void);
static void ScrollPlace (void);
static void CheckCurrentTile (void);
static void CountGameplayHelpMove (void);
static void StartWinOuttro (void);
static char OuttroLook (
    int dx,
    int dy
);
static int RandomRange (
    int min_value,
    int max_value
);
static int RandomOuttroMoveDelay (void);
static void ScheduleOuttroTimer (
    int delay_ms
);
static int IsPassable (
    char ch
);
char GetCell (
    int row,
    int col1
);
static void SetCell (
    int  row,
    int  col1,
    char ch
);
static void AtVillage (void);
static void ClearLog (void);
static void AppendLog (
    const char *text
);

/* =========
** init game
** =========
*/
void InitGame (void)
{
    /* local vars
    */
    /* reset timers
    */
    KillTimer (
        g_hwnd,
        TIMER_OUTTRO_START
    );
    KillTimer (
        g_hwnd,
        TIMER_OUTTRO
    );
    StopMusic();
    /* start music
    */
    PlayMusic("uyworld.mp3");

    /* reset state
    */
    g_on_title = 0;
    g_timer_on = 1;
    g_won = 0;
    g_lost = 0;
    g_outtro_on = 0;
    g_outtro_last_dx = 0;
    g_outtro_last_dy = 0;
    g_outtro_steps_until_pause = 0;
    g_found_scroll = 0;
    g_talked_to_sensei = 0;
    g_time_left = 100;
    g_char_x = 5;
    g_char_y = 5;
    g_char_face = 1;
    g_gameplay_help_moves = 0;
    g_gameplay_help_visible = 1;

    /* reset world
    */
    InitMap();
    ClearLog();
    AppendLog("A quiet wind moves across Booken Island.");
    ScrollPlace();
    CheckCurrentTile();

    SetTimer (
        g_hwnd,
        TIMER_CLOCK,
        1000,
        NULL
    );
    InvalidateRect (
        g_hwnd,
        NULL,
        FALSE
    );
}

/* ========
** init map
** ========
*/
static void InitMap (void)
{
    /* local vars
    */
    int i;

    for (i = 0; i < MAP_H; i++) {
        lstrcpynA (
            g_map[i],
            g_original_map[i],
            MAP_W + 1
        );
    }
}

/* ============
** place scroll
** ============
*/
static void ScrollPlace (void)
{
    /* local vars
    */
    int sx;
    int sy;
    int tries;

    tries = 0;
    /* find random passable cell
    */
    do {
        sx = (rand() % MAP_W) + 1; // vb mid column, 1-based
        sy = (rand() % 35) + 1;    // original used rows
                                   //  1 through 35
        tries++;
    } while (tries < 10000 && GetCell(sy, sx) != '6');

    if (GetCell(sy, sx) == '6') {
        SetCell (
            sy,
            sx,
            'N'
        );
    }
}

/* ===========
** move player
** ===========
*/
void MovePlayer (
    int dx,
    int dy,
    int face)
{
    /* local vars
    */
    char look;

    /* assume blocked
    */
    look = 'V';

    /* inspect edge
    */
    if (dx == 0 && dy == -1) {
        look = GetCell (
            g_char_y,
            g_char_x + 1
        );
    } else if (dx == 0 && dy == 1) {
        look = GetCell (
            g_char_y + 2,
            g_char_x + 1
        );
    } else if (dx == -1 && dy == 0) {
        look = GetCell (
            g_char_y + 1,
            g_char_x
        );
    } else if (dx == 1 && dy == 0) {
        look = GetCell (
            g_char_y + 1,
            g_char_x + 2
        );
    }

    /* move if clear
    */
    if (IsPassable(look)) {
        g_char_x += dx;
        g_char_y += dy;
        g_char_face = face;

        /* count help move
        */
        CountGameplayHelpMove();

        /* resolve tile
        */
        CheckCurrentTile();
    } else {
        g_char_face = face;
    }

    /* redraw
    */
    InvalidateRect (
        g_hwnd,
        NULL,
        FALSE
    );
}

/* ========================
** gameplay help visibility
** ========================
*/
int IsGameplayHelpVisible (void)
{
    /* return state
    */
    return g_gameplay_help_visible;
}

/* =========================
** count gameplay help move
** =========================
*/
static void CountGameplayHelpMove (void)
{
    /* check state
    */
    if (!g_gameplay_help_visible) {
        return;
    }

    /* count move
    */
    g_gameplay_help_moves++;

    /* hide help
    */
    if (g_gameplay_help_moves >= GAMEPLAY_HELP_MOVE_LIMIT) {
        g_gameplay_help_visible = 0;
    }
}

/* ==========
** check tile
** ==========
*/
static void CheckCurrentTile (void)
{
    /* local vars
    */
    char look;

    look = GetCell (
        g_char_y + 1,
        g_char_x + 1
    );

    if (look == 'N') {
        if (!g_found_scroll) {
            g_found_scroll = 1;
            SetCell (
                g_char_y + 1,
                g_char_x + 1,
                '6'
            );
            AppendLog (
                "You have found the Sacred Scroll!"
            );
        }
    } else if (look == 'U') {
        AtVillage();
    }
}

/* ================
** start win outtro
** ================
*/
static void StartWinOuttro (void)
{
    /* local vars
    */
    /* stop old timers
    */
    KillTimer (
        g_hwnd,
        TIMER_OUTTRO_START
    );
    KillTimer (
        g_hwnd,
        TIMER_OUTTRO
    );

    g_outtro_on = 1;
    g_char_face = 1;
    g_outtro_last_dx = 0;
    g_outtro_last_dy = 0;
    g_outtro_steps_until_pause = 0;
    AppendLog (
        "Victory! Press ESC to return to the title screen.");
    SetTimer (
        g_hwnd,
        TIMER_OUTTRO_START,
        OUTTRO_START_DELAY_MS,
        NULL
    );
    InvalidateRect (
        g_hwnd,
        NULL,
        FALSE
    );
}

/* =================
** begin outtro walk
** =================
*/
void BeginWinOuttroWalk (void)
{
    /* check state
    */
    if (!g_outtro_on) {
        return;
    }

    /* stop start timer
    */
    KillTimer (
        g_hwnd,
        TIMER_OUTTRO_START
    );

    /* announce walk
    */
    AppendLog (
        "The samurai walks on."
    );

    /* set walk run
    */
    g_outtro_steps_until_pause = RandomRange (
        OUTTRO_STEPS_MIN,
        OUTTRO_STEPS_MAX
    );

    /* schedule walk
    */
    ScheduleOuttroTimer (
        RandomOuttroMoveDelay()
    );

    /* redraw frame
    */
    InvalidateRect (
        g_hwnd,
        NULL,
        FALSE
    );
}

/* ================
** look outtro cell
** ================
*/
static char OuttroLook (
    int dx,
    int dy)
{
    /* look up
    */
    if (
        dx == 0 &&
        dy == -1) {
        return GetCell (
            g_char_y,
            g_char_x + 1
        );
    }

    /* look down
    */
    if (
        dx == 0 &&
        dy == 1) {
        return GetCell (
            g_char_y + 2,
            g_char_x + 1
        );
    }

    /* look left
    */
    if (
        dx == -1 &&
        dy == 0) {
        return GetCell (
            g_char_y + 1,
            g_char_x
        );
    }

    /* look right
    */
    if (
        dx == 1 &&
        dy == 0) {
        return GetCell (
            g_char_y + 1,
            g_char_x + 2
        );
    }

    /* block unknown
    */
    return 'V';
}

/* ===========
** outtro step
** ===========
*/
void OuttroStep (void)
{
    /* local vars
    */
    int dxs[4]   = { 0, 1, 0, -1 }; // up, right, down, left
    int dys[4]   = { -1, 0, 1, 0 }; // up, right, down, left
    int faces[4] = { 2, 4, 1, 3 };  // matching sprite face
    int viable[4];
    int preferred[4];
    int viable_count;
    int preferred_count;
    int chosen;
    int next_delay;
    int i;

    /* check state
    */
    if (!g_outtro_on) {
        return;
    }

    /* stop active timer
    */
    KillTimer (
        g_hwnd,
        TIMER_OUTTRO
    );

    /* clear choice
    */
    chosen = -1;

    /* keep same direction
    */
    if (
        g_outtro_last_dx != 0 ||
        g_outtro_last_dy != 0) {
        for (i = 0; i < 4; i++) {
            if (
                dxs[i] == g_outtro_last_dx &&
                dys[i] == g_outtro_last_dy) {
                if (
                    IsPassable (
                        OuttroLook (
                            dxs[i],
                            dys[i]
                        )
                    ) &&
                    (rand() % 3) != 0) {
                    chosen = i;
                }
                break;
            }
        }
    }

    /* find new direction
    */
    if (chosen < 0) {
        viable_count    = 0;
        preferred_count = 0;

        /* scan directions
        */
        for (i = 0; i < 4; i++) {
            if (
                IsPassable (
                    OuttroLook (
                        dxs[i],
                        dys[i]
                    )
                )
            ) {
                viable[viable_count] = i;
                viable_count++;

                /* avoid reverse
                */
                if (!(
                    dxs[i] == -g_outtro_last_dx &&
                    dys[i] == -g_outtro_last_dy)) {
                    preferred[preferred_count] = i;
                    preferred_count++;
                }
            }
        }

        /* choose direction
        */
        if (preferred_count > 0) {
            chosen = preferred[rand() % preferred_count];
        } else if (viable_count > 0) {
            chosen = viable[rand() % viable_count];
        }
    }

    /* move samurai
    */
    if (chosen >= 0) {
        g_char_x        += dxs[chosen];
        g_char_y        += dys[chosen];
        g_char_face      = faces[chosen];
        g_outtro_last_dx = dxs[chosen];
        g_outtro_last_dy = dys[chosen];
    } else {
        g_char_face++;
        if (g_char_face > 4) {
            g_char_face = 1;
        }
    }

    /* count step
    */
    g_outtro_steps_until_pause--;

    /* maybe pause
    */
    if (g_outtro_steps_until_pause <= 0) {
        g_outtro_steps_until_pause = RandomRange (
            OUTTRO_STEPS_MIN,
            OUTTRO_STEPS_MAX
        );
        next_delay = RandomRange (
            OUTTRO_PAUSE_MIN_MS,
            OUTTRO_PAUSE_MAX_MS
        );
    } else {
        next_delay = RandomOuttroMoveDelay();
    }

    /* redraw frame
    */
    InvalidateRect (
        g_hwnd,
        NULL,
        FALSE
    );

    /* schedule next step
    */
    ScheduleOuttroTimer (
        next_delay
    );
}

/* ============
** random range
** ============
*/
static int RandomRange (
    int min_value,
    int max_value)
{
    /* local vars
    */
    int temp;

    if (max_value < min_value) {
        temp = min_value;
        min_value = max_value;
        max_value = temp;
    }

    if (max_value == min_value) {
        return min_value;
    }

    return min_value +
        (rand() % (max_value - min_value + 1));
}

/* ===================
** random outtro delay
** ===================
*/
static int RandomOuttroMoveDelay (void)
{
    /* return delay
    */
    return RandomRange (
        OUTTRO_MOVE_MS - OUTTRO_MOVE_JITTER_MS,
        OUTTRO_MOVE_MS + OUTTRO_MOVE_JITTER_MS
    );
}

/* ===============
** schedule outtro
** ===============
*/
static void ScheduleOuttroTimer (
    int delay_ms)
{
    /* local vars
    */
    if (!g_outtro_on) {
        return;
    }

    if (delay_ms < 1) {
        delay_ms = 1;
    }

    KillTimer (
        g_hwnd,
        TIMER_OUTTRO
    );
    SetTimer (
        g_hwnd,
        TIMER_OUTTRO,
        (UINT)delay_ms,
        NULL
    );
}

/* ===============
** return to title
** ===============
*/
void ReturnToTitle (void)
{
    /* stop timers
    */
    KillTimer (
        g_hwnd,
        TIMER_CLOCK
    );
    KillTimer (
        g_hwnd,
        TIMER_OUTTRO_START
    );
    KillTimer (
        g_hwnd,
        TIMER_OUTTRO
    );

    g_on_title                 = 1;
    g_timer_on                 = 0;
    g_outtro_on                = 0;
    g_outtro_last_dx           = 0;
    g_outtro_last_dy           = 0;
    g_outtro_steps_until_pause = 0;
    g_won                      = 0;
    g_lost                     = 0;
    g_talked_to_sensei         = 0;

    StopMusic();
    
    PlayMusic (
        "kungfu.mp3"
    );
    
    InvalidateRect (
        g_hwnd,
        NULL,
        FALSE
    );
}

/* ===========
** is passable
** ===========
*/
static int IsPassable (
    char ch)
{
    /* return passable
    */
    return ch == '6' ||
           ch == '3' ||
           ch == 'U' ||
           ch == 'N';
}

/* ============
** get map cell
** ============
*/
char GetCell (
    int row,
    int col1)
{
    /* local vars
    */
    if (
        row < 0      ||
        row >= MAP_H ||
        col1 < 1     ||
        col1 > MAP_W) {
        return 'V';
    }
    return g_map[row][col1 - 1];
}

/* ============
** set map cell
** ============
*/
static void SetCell (
    int  row,
    int  col1,
    char ch)
{
    /* local vars
    */
    if (
        row < 0      ||
        row >= MAP_H ||
        col1 < 1     ||
        col1 > MAP_W) {
        return;
    }
    g_map[row][col1 - 1] = ch;
}

/* ==============
** talk to others
** ==============
*/
void TalkToOthers (void)
{
    /* local vars
    */
    char look;
    const char *speaker;
    int spoke;
    char line[512];

    look = GetCell (
        g_char_y,
        g_char_x + 1
    );
    speaker = "";
    spoke = 0;

    switch (look) {
    case '7':
        spoke = 1;
        speaker = "Haji:";
        break;
    case '8':
        spoke = 1;
        speaker = "Mail-Man Dan:";
        break;
    case '9':
        spoke = 1;
        speaker = "Nezumi:";
        break;
    case 'A':
    case 'a':
        spoke = 1;
        speaker = "Shinji:";
        break;
    case 'B':
        spoke = 1;
        speaker = "Cocoa:";
        break;
    case 'O':
        spoke = 1;
        speaker = "The Sign Reads:";
        break;
    }

    if (spoke) {
        AppendLog (
            "You: Konnichiwa! Do you know where "
            "I can find the Sacred Scroll?"
        );
        wsprintfA (
            line,
            "%s %s",
            speaker,
            g_answers[rand() % 10]
        );
        AppendLog(line);
    } else {
        AppendLog("Nobody is there to speak! to");
    }
}

/* ==========
** at village
** ==========
*/
static void AtVillage (void)
{
    /* local vars
    */
    if (g_found_scroll) {
        if (!g_won) {
            g_talked_to_sensei = 1;
            AppendLog (
                "You: Master I have found "
                "the Sacred Scroll!"
            );
            AppendLog(
                "Sensei: Good work! You "
                "are truly a worthy Samurai!"
            );
            g_timer_on = 0;
            g_won = 1;
            KillTimer (
                g_hwnd,
                TIMER_CLOCK
            );
            StopMusic();
            PlayMusic("lnbintro.mp3");
            StartWinOuttro();
        }
    } else {
        if (!g_talked_to_sensei) {
            g_talked_to_sensei = 1;
            AppendLog (
                "Sensei: I am dying. "
                "My life is fading quickly."
            );
            AppendLog (
                "Sensei: Only the Sacred Scroll can save me. "
                "Find it before my time runs out."
            );
            AppendLog (
                "You: I will find it, Master."
            );
        } else {
            AppendLog (
                "Sensei: Have you found the scroll yet?"
            );
            AppendLog (
                "You: Not yet, this is a difficult task."
            );
        }
    }
}

/* =========
** lose game
** =========
*/
void LoseGame (void)
{
    /* local vars
    */
    if (g_lost) {
        return;
    }

    g_lost = 1;
    g_timer_on = 0;
    g_outtro_on = 0;
    g_char_face = 1;
    /* stop game timers
    */
    KillTimer (
        g_hwnd,
        TIMER_CLOCK
    );
    KillTimer (
        g_hwnd,
        TIMER_OUTTRO_START
    );
    KillTimer (
        g_hwnd,
        TIMER_OUTTRO
    );
    StopMusic();
    /* play loss music
    */
    PlayMusic("haruumi1.mp3");

    if (g_talked_to_sensei) {
        AppendLog (
            "You: Oh no! I am too late! I have failed."
        );
        AppendLog (
            "You: My only hope is through Seppuku!"
        );
    } else {
        AppendLog (
            "A sickening feeling comes over you."
        );
        AppendLog (
            "Something terrible must have happened."
        );
        AppendLog (
           "Game over."
        );
    }

    AppendLog (
        "Press ESC to return to the title screen."
    );
    InvalidateRect (
        g_hwnd,
        NULL,
        FALSE
    );
}

/* =========
** clear log
** =========
*/
static void ClearLog (void)
{
    /* local vars
    */
    int i;

    for (i = 0; i < MAX_LOG_LINES; i++) {
        g_log[i][0] = 0;
    }
    g_log_count = 0;
}

/* ==========
** append log
** ==========
*/
static void AppendLog (
    const char *text)
{
    /* local vars
    */
    /* make room
    */
    if (g_log_count >= MAX_LOG_LINES) {
        int i;
        for (i = 1; i < MAX_LOG_LINES; i++) {
            lstrcpynA (
                g_log[i - 1],
                g_log[i],
                MAX_LOG_LEN
            );
        }
        g_log_count = MAX_LOG_LINES - 1;
    }

    lstrcpynA (
        g_log[g_log_count],
        text,
        MAX_LOG_LEN
    );
    g_log_count++;
}
