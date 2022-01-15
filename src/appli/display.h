#include "stdbool.h"

#ifndef DISPLAY_H
#define DISPLAY_H

void DISPLAY_home_screen(void); // fonction permettant d'initialiser l'affichage sur l'�cran au d�marrage
void DISPLAY_init(void);
void DISPLAY_2_choices(void);
void DISPLAY_set_choice_made(bool value);
bool DISPLAY_get_choice_made(void);
void DISPLAY_percentvotetot(void);

#endif // DISPLAY_H
