#include "display.h"
#include "tft_ili9341/stm32f1_ili9341.h"
#include "appli.h"
#include "percentage.h"

#include <stdbool.h>
#include "macro_types.h"

typedef enum{
	INIT,
	BASE,
	DETERMINATE_TOP,
	DETERMINATE_DOWN,
	CURSOR_TOP,
	CURSOR_DOWN,
	NOTHING_TOP,
	NOTHING_DOWN,
	CHOICE_1,
	CHOICE_2

}ecran_e;

/*-----------------------------------------PRIVATE VARIABLES-------------------------------------*/
static bool choice;
/*------------------------------------PRIVATE FUNCTIONS PROTOTYPES-------------------------------*/
static void DISPLAY_draw_top_cursor(void);
static void DISPLAY_draw_down_cursor(void);
static void DISPLAY_erase_cursor(void);
static void DISPLAY_erase_screen(void);
static void DISPLAY_background_choice(void);
static void DISPLAY_percentvote1(void); //Permet d'afficher le pourcentage de voix pour le choix 1
static void DISPLAY_percentvote2(void); //Permet d'afficher le pourcentage de voix pour le choix 2
/*-----------------------------------------------------------------------------------------------*/

void DISPLAY_home_screen(void){
	ILI9341_Fill(ILI9341_COLOR_WHITE);
	ILI9341_printf(55,80, &Font_16x26, ILI9341_COLOR_RED,ILI9341_COLOR_WHITE,"E");
	ILI9341_printf(70,80, &Font_16x26, ILI9341_COLOR_BLUE,ILI9341_COLOR_WHITE, "SEObjectivity");
	ILI9341_printf(18,135, &Font_11x18, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE,"Appuyez sur le bouton VERT");
	ILI9341_printf(90,165, &Font_11x18, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE,"pour demarrer");
}

void DISPLAY_init(void){
	ILI9341_Init();
	ILI9341_Fill(ILI9341_COLOR_WHITE);
	ILI9341_Rotate(ILI9341_Orientation_Landscape_1);
}

void DISPLAY_background_choice(void){
	ILI9341_Rotate(ILI9341_Orientation_Landscape_1);
	ILI9341_Fill(ILI9341_COLOR_WHITE);
	ILI9341_printf(17,20, &Font_11x18, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE, "CHOISISSEZ UNE PROPOSITION");
	ILI9341_printf(125, 90, &Font_11x18, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE, "CHOIX 1");
	ILI9341_printf(125, 170, &Font_11x18, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE, "CHOIX 2");
	ILI9341_DrawLine(17,40,302,40,ILI9341_COLOR_RED);
}

void DISPLAY_draw_top_cursor(void){
	ILI9341_DrawFilledCircle(100,97,8,ILI9341_COLOR_RED);
}

void DISPLAY_draw_down_cursor(void){
	ILI9341_DrawFilledCircle(100,177,8,ILI9341_COLOR_RED);
}

void DISPLAY_erase_cursor(void){
	ILI9341_DrawFilledRectangle(80,87,120,187,ILI9341_COLOR_WHITE);
}

void DISPLAY_erase_screen(void){
	ILI9341_Fill(ILI9341_COLOR_WHITE);
}

void DISPLAY_2_choices(void){
	static ecran_e state = INIT;
		switch(state){
			case INIT :
				DISPLAY_background_choice();
				state = BASE;
				break;
			case BASE :
				DISPLAY_draw_top_cursor();
				state = DETERMINATE_TOP;
				break;
			case DETERMINATE_TOP :
				if (right_button_press_event()){
					state = NOTHING_TOP;
				}
				if (left_button_press_event()){
					state = CURSOR_DOWN;
				}
				if (center_button_press_event()){
					state = CHOICE_1;
				}
				break;
			case DETERMINATE_DOWN :
				if (right_button_press_event()){
					state = CURSOR_TOP;
				}
				if (left_button_press_event()){
					state = NOTHING_DOWN;
				}
				if (center_button_press_event()){
					state = CHOICE_2;
				}
				break;
			case CURSOR_DOWN :
				DISPLAY_erase_cursor();
				DISPLAY_draw_down_cursor();
				state = DETERMINATE_DOWN;
				break;
			case CURSOR_TOP :
				DISPLAY_erase_cursor();
				DISPLAY_draw_top_cursor();
				state = DETERMINATE_TOP;
				break;
			case NOTHING_TOP :
				state = DETERMINATE_TOP;
				break;
			case NOTHING_DOWN :
				state = DETERMINATE_DOWN;
				break;
			case CHOICE_1 :
				DISPLAY_erase_screen();
				ILI9341_printf(50,60, &Font_7x10, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE, "VOUS AVEZ CHOISI LE CHOIX 1");
				choice = true;
				PERCENTAGE_add_vote_1();
				state = INIT;
				break;
			case CHOICE_2 :
				DISPLAY_erase_screen();
				ILI9341_printf(50,60, &Font_7x10, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE, "VOUS AVEZ CHOISI LE CHOIX 2");
				choice = true;
				PERCENTAGE_add_vote_2();
				state = INIT;
				break;
			default :
				break;
		}

}

void DISPLAY_percentvote1(void){
	ILI9341_printf(17,20, &Font_11x18, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE, "Pourcentage Choix 1 : %d %%",PERCENTAGE_get_percentage_1());
	if(PERCENTAGE_graph() != 0){
		ILI9341_DrawFilledRectangle(30,150,PERCENTAGE_graph(),180,ILI9341_COLOR_BLUE);
	}


}

void DISPLAY_percentvote2(void){
	ILI9341_printf(17,60, &Font_11x18, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE, "Pourcentage Choix 2 : %d %%",PERCENTAGE_get_percentage_2());
	if(PERCENTAGE_get_percentage_2() != 0){
		ILI9341_DrawFilledRectangle(30,150,290,180,ILI9341_COLOR_RED);
	}

}

void DISPLAY_percentvotetot(void){
	PERCENTAGE_compute_all();
	DISPLAY_percentvote2();
	DISPLAY_percentvote1();
	ILI9341_printf(17,100, &Font_11x18, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE, "Nombre de votes total : %d",PERCENTAGE_get_total_vote());
}

/*-----------------------------------------ACCESSEUR(S)-------------------------------------*/
void DISPLAY_set_choice_made(bool value){
	if(value)
		choice = true;
	else
		choice = false;
}

bool DISPLAY_get_choice_made(void){
	return choice;
}
// Pour 3 propositions de vote

/*ILI9341_printf(125, 70, &Font_11x18, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE, "CHOIX 1");
ILI9341_printf(125, 130, &Font_11x18, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE, "CHOIX 2");
ILI9341_printf(125, 190, &Font_11x18, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE, "CHOIX 3");
ILI9341_DrawFilledCircle(100,77,8,ILI9341_COLOR_RED);
ILI9341_DrawFilledCircle(100,137,8,ILI9341_COLOR_RED);
ILI9341_DrawFilledCircle(100,197,8,ILI9341_COLOR_RED);*/

// Pour 4 propositions de vote

/*ILI9341_printf(125, 70, &Font_11x18, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE, "CHOIX 1");
ILI9341_printf(125, 110, &Font_11x18, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE, "CHOIX 2");
ILI9341_printf(125, 150, &Font_11x18, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE, "CHOIX 3");
ILI9341_printf(125, 190, &Font_11x18, ILI9341_COLOR_BLACK,ILI9341_COLOR_WHITE, "CHOIX 4");
ILI9341_DrawFilledCircle(100,77,8,ILI9341_COLOR_RED);
ILI9341_DrawFilledCircle(100,117,8,ILI9341_COLOR_RED);
ILI9341_DrawFilledCircle(100,157,8,ILI9341_COLOR_RED);
ILI9341_DrawFilledCircle(100,197,8,ILI9341_COLOR_RED);*/



