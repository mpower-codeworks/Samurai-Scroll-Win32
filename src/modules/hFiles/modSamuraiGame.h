/*
    / / / / / / / / / / / / /
  / / modSamuraiGame.h  / /
/ / / / / / / / / / / / /

map movement timer and story logic

*/

#ifndef MODSAMURAIGAME_H
#define MODSAMURAIGAME_H

#include "modSamuraiCommon.h"

extern int  g_on_title;
extern int  g_timer_on;
extern int  g_won;
extern int  g_lost;
extern int  g_outtro_on;
extern int  g_outtro_last_dx;
extern int  g_outtro_last_dy;
extern int  g_outtro_steps_until_pause;
extern int  g_found_scroll;
extern int  g_talked_to_sensei;
extern int  g_time_left;
extern int  g_char_x;
extern int  g_char_y;
extern int  g_char_face;
extern char g_map[MAP_H][MAP_W + 1];
extern char g_log[MAX_LOG_LINES][MAX_LOG_LEN];
extern int  g_log_count;

void InitGame (void);

void MovePlayer (
    int dx,
    int dy,
    int face
);

char GetCell (
    int row,
    int col1
);

int IsGameplayHelpVisible (void);

void TalkToOthers (void);
void BeginWinOuttroWalk (void);
void OuttroStep (void);
void ReturnToTitle (void);
void LoseGame (void);

#endif
