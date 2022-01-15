#include <stdbool.h>
#include "stdio.h"

#ifndef VOTE_MODE_H
#define VOTE_MODE_H

void VOTE_MODE_management(void);
void VOTE_MODE_display(void);
bool VOTE_MODE_get_status(void);
void VOTE_MODE_set_status(bool status);
void VOTE_MODE_set_VOTE_MODE_DISPLAY_FLAG(bool status);
void VOTE_MODE_set_vote_mode_display_timer(uint32_t value);
uint32_t VOTE_MODE_get_vote_mode_display_timer(void);
void VOTE_MODE_vote_mode_dec_display_timer(void);

#endif
