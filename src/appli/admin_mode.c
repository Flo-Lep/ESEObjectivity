#include "admin_mode.h"
#include "stdbool.h"
#include "vote_mode.h"
#include "admin_mode.h"
#include "stm32f1_ili9341.h"
#include "appli.h"
#include "display.h"

typedef enum{
	ADMIN_INIT,
	SETTING_1,
	SETTING_2,
	RUNNING_VOTE,
	SET_A_VOTE,
}admin_mode_e;

/*-----------------------------------------PRIVATE VARIABLES-------------------------------------*/
static admin_mode_e current_admin_mode;
static bool admin_display_update;
static bool previous_admin_mode;
/*------------------------------------PRIVATE FUNCTIONS PROTOTYPES-------------------------------*/
static void ADMIN_MODE_init(void);
/*-----------------------------------------------------------------------------------------------*/
void ADMIN_MODE_init(void){
	admin_display_update= true;
	current_admin_mode = ADMIN_INIT;
}

void ADMIN_MODE_management(void){
	static admin_mode_e admin_mode = ADMIN_INIT;
	previous_admin_mode = admin_mode;
	if(VOTE_MODE_get_status()){
		admin_mode = RUNNING_VOTE;
		current_admin_mode = admin_mode;
		if(previous_admin_mode != current_admin_mode)
			admin_display_update = true;
	}
	else{
		admin_mode = SET_A_VOTE;
		current_admin_mode = admin_mode;
		if(previous_admin_mode != current_admin_mode)
			admin_display_update = true;
	}
	switch(admin_mode){
	case ADMIN_INIT :
		ADMIN_MODE_init();
		admin_mode =  SET_A_VOTE;
		current_admin_mode = admin_mode;
		break;
	case SETTING_1 :
		//Inutile pour le moment
		break;
	case SETTING_2 :
		//Inutile pour le moment
		break;
	case RUNNING_VOTE :
		if(center_button_press_event())
		{
			DISPLAY_init(); //A revoir
			DISPLAY_percentvotetot();
			HAL_Delay(5000); //Vous nous pardonnerez...
			VOTE_MODE_set_status(false);
			admin_mode = ADMIN_INIT;
			current_admin_mode = admin_mode;
			APPLI_set_go_back_to_appli_init(true);
		}
		break;
	case SET_A_VOTE :
		if(center_button_press_event())
		{
			VOTE_MODE_set_status(true);
			admin_mode = ADMIN_INIT;
			current_admin_mode = admin_mode;
			APPLI_set_go_back_to_home_screen(true);
		}
		break;
	default :
		break;
	}
}

void ADMIN_MODE_display(void){
	 if(admin_display_update){
		 switch(current_admin_mode){
			case ADMIN_INIT :
				//
				break;
			case SETTING_1 :
				ILI9341_Fill(ILI9341_COLOR_WHITE);
				ILI9341_printf(50,50,&Font_7x10,ILI9341_COLOR_RED, ILI9341_COLOR_WHITE,"SETTING 1");
				break;
			case SETTING_2 :
				ILI9341_Fill(ILI9341_COLOR_WHITE);
				ILI9341_printf(50,50,&Font_7x10,ILI9341_COLOR_RED, ILI9341_COLOR_WHITE,"SETING 2");
				break;
			case RUNNING_VOTE :
				ILI9341_Fill(ILI9341_COLOR_WHITE);
				ILI9341_printf(45,50,&Font_7x10,ILI9341_COLOR_RED, ILI9341_COLOR_WHITE,"UN VOTE EST DÉJÀ EN COURS :");
				ILI9341_printf(30,100,&Font_7x10,ILI9341_COLOR_RED, ILI9341_COLOR_WHITE,"Appuyez sur le bouton central pour terminer le vote");
				ILI9341_printf(30,150,&Font_7x10,ILI9341_COLOR_RED, ILI9341_COLOR_WHITE,"Appuyez sur la télécommande pour continuer le vote");
				break;
			case SET_A_VOTE :
				ILI9341_Fill(ILI9341_COLOR_WHITE);
				ILI9341_printf(45,50,&Font_7x10,ILI9341_COLOR_RED, ILI9341_COLOR_WHITE,"AUCUN VOTE N'EST EN COURS :");
				ILI9341_printf(30,100,&Font_7x10,ILI9341_COLOR_RED, ILI9341_COLOR_WHITE,"Appuyez sur le bouton central pour démarrer une session ?");
				ILI9341_printf(30,150,&Font_7x10,ILI9341_COLOR_RED, ILI9341_COLOR_WHITE,"Appuyez sur la télécommande pour revenir à l'écran d'accueil");
				break;
			default :
				ILI9341_printf(50,50,&Font_7x10,ILI9341_COLOR_RED, ILI9341_COLOR_WHITE,"Erreur dans l'affichage du mode admin");
				break;
			}
		 admin_display_update= false;
	 }

 }
