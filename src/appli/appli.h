#include "macro_types.h"
#include <stdbool.h>

#ifndef APPLI_MANAGEMENT_H
#define APPLI_MANAGEMENT_H

void APPLI_process_main(void);
void APPLI_set_go_back_to_home_screen(bool value);
void APPLI_set_go_back_to_appli_init(bool value);
bool_e center_button_press_event(void);
bool_e left_button_press_event(void);
bool_e right_button_press_event(void);
uint16_t process_ms(void);
#endif
