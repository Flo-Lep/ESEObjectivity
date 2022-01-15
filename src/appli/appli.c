#include "appli.h"
#include <stdbool.h>
#include "macro_types.h"
#include "config.h"
#include "stm32f1_ili9341.h"
#include "flash_memory_management.h"
#include "vote_mode.h"
#include "admin_mode.h"
#include "nfc_sensor.h"
#include "display.h"
#include "stm32f1_gpio.h"
#include "stm32f1_extit.h"
#include "ir/ir_receiver.h"

typedef enum{
	INIT,
	HOME_SCREEN,
	ADMIN_MODE,
	VOTE_MODE,
}mode_e;

/*-----------------------------------------PRIVATE VARIABLES-------------------------------------*/
static mode_e current_appli_mode;
static bool display_update;
static bool FLAG_IT;
static volatile uint32_t timer_extit = 0;
static bool extit_change_mode;
static bool go_back_to_home_screen;
static bool go_back_to_appli_init;
/*------------------------------------PRIVATE FUNCTIONS PROTOTYPES-------------------------------*/
static void APPLI_display(void);
static void APPLI_init(void);
static void APPLI_init_peripherals(void);
static void APPLI_EXTIT_init(void);
static void APPLI_EXTIT_command(void);
mode_e APPLI_EXTIT_get_new_mode(void); //On pref laisser l'enum ds le .c d'où la présence de ce prototype
/*-----------------------------------------------------------------------------------------------*/

void APPLI_init_peripherals(void){
	//Initialisation du bouton gauche
	BSP_GPIO_PinCfg(LEFT_BUTTON_GPIO, LEFT_BUTTON_PIN, GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);
	//Initialisation du bouton central
	BSP_GPIO_PinCfg(CENTER_BUTTON_GPIO, CENTER_BUTTON_PIN, GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);
	//Initialisation du bouton droit
	BSP_GPIO_PinCfg(RIGHT_BUTTON_GPIO, RIGHT_BUTTON_PIN, GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);
	//Initialisation du buzzer
	BSP_GPIO_PinCfg(BUZZER_GPIO, BUZZER_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN, GPIO_SPEED_FREQ_HIGH);
}

void APPLI_init(void){
	NFC_SENSOR_init();
	DISPLAY_init();
	APPLI_init_peripherals();
	FLASH_MEMORY_MANAGEMENT_init(); //En théorie si on veut limiter l'usure de la flash il ne faudrait appeler cette fonction
	APPLI_EXTIT_init();				//que lorsque qu'ont veut apporter des modifs pour ne pas réécrire dedans inutilement
	PERCENTAGE_init();
	display_update = false;
	extit_change_mode = false;
	go_back_to_home_screen = false;
	FLAG_IT = false;
}

void APPLI_process_main(void){
	static mode_e state = INIT;
	if(extit_change_mode){
		state = APPLI_EXTIT_get_new_mode();
		current_appli_mode = state;
		extit_change_mode = false;
	}
	if(go_back_to_home_screen){
		state = HOME_SCREEN;
		current_appli_mode = state;
		go_back_to_home_screen = false;
	}
	if(go_back_to_appli_init){
		state = INIT;
		current_appli_mode = state;
		go_back_to_appli_init = false;
	}
	switch(state){
		case INIT :
			APPLI_init();
			state = HOME_SCREEN;
			current_appli_mode = state;
			display_update = true;
			break;
		case HOME_SCREEN :
			APPLI_display();
			if(center_button_press_event()){
				if(VOTE_MODE_get_status()){
					state = VOTE_MODE;
					current_appli_mode = state;
					display_update = true;
				}
				else{
					ILI9341_printf(20,20,&Font_7x10,ILI9341_COLOR_RED, ILI9341_COLOR_WHITE,"AUCUN VOTE N'EST EN COURS FAITES APPEL A");
					ILI9341_printf(95,35,&Font_7x10,ILI9341_COLOR_RED, ILI9341_COLOR_WHITE,"UN ADMINISTRATEUR");
				}
			}
			break;
		case ADMIN_MODE :
			ADMIN_MODE_management();
			ADMIN_MODE_display();
			break;
		case VOTE_MODE :
			VOTE_MODE_management();
			VOTE_MODE_display();
			break;
		default :
			break;
	}
}


void APPLI_display(void){
	if(display_update){
		display_update = false;
		switch(current_appli_mode){
			case INIT :
				//On affiche rien comme c'est pas censé arriver...
				ILI9341_printf(50,50,&Font_7x10,ILI9341_COLOR_RED, ILI9341_COLOR_WHITE,"Error ! case INIT in display_mode_management");
				while(1);
				break;
			case HOME_SCREEN :
				DISPLAY_home_screen();
				break;
			case ADMIN_MODE :
				//
			case VOTE_MODE :
				//
				break;
			default :
				ILI9341_printf(50,50,&Font_7x10,ILI9341_COLOR_RED, ILI9341_COLOR_WHITE,"Error ! case default in display_mode_management");
				break;
		}
	}

}
/*-----------------------------------------ACCESSEURS-------------------------------------*/
void APPLI_set_go_back_to_home_screen(bool value){
	if(value)
		go_back_to_home_screen = true;
	else
		go_back_to_home_screen = false;
}
void APPLI_set_go_back_to_appli_init(bool value){
	if(value)
		go_back_to_appli_init = true;
	else
		go_back_to_appli_init = false;
}
/*-----------------------------------------EXTIT IR-------------------------------------*/
/*
  * @brief Cette fonction initialise et autorise les interruptions externes pour la broche IR_RECEIVER (PB8)
  */
void APPLI_EXTIT_init(void)
{
	BSP_GPIO_PinCfg(IR_RECEIVER_GPIO, IR_RECEIVER_PIN, GPIO_MODE_IT_FALLING,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);
	EXTIT_set_callback(&APPLI_EXTIT_command, EXTI_gpiopin_to_pin_number(IR_RECEIVER_PIN), TRUE);
	EXTIT_enable(0);
}

void APPLI_EXTIT_command(void)
{
	if(FLAG_IT)
	{
		extit_change_mode = true;
		FLAG_IT = FALSE;
	}
}

mode_e APPLI_EXTIT_get_new_mode(void){
	mode_e ret = INIT;
	if(current_appli_mode == HOME_SCREEN){
			ret = ADMIN_MODE;
	}
	else if(current_appli_mode == ADMIN_MODE){
		ret = HOME_SCREEN;
	}
	else if(current_appli_mode == VOTE_MODE){
		ret = ADMIN_MODE;
	}
	display_update = true;
	return ret;
}
/*-----------------------------------------USEFUL FUNCTIONS-------------------------------------*/
/*
 * @brief Ces fonctions qui d�tectent l'appui sur un des 3 boutons
 * @pre	  Le bouton doit �tre initialis� selon une broche en amont (ds le main par ex)
 * @author : Nirgal
 */
bool_e center_button_press_event(void)
{
	static bool_e previous_button = false; //état précédent du bouton
	bool_e ret = false;
	bool_e current_button; //état actuel du bouton
	//bouton en logique inverse, d'où le '!'
	current_button = !HAL_GPIO_ReadPin(CENTER_BUTTON_GPIO, CENTER_BUTTON_PIN);
	//si le bouton est appuyé et ne l'était pas avant, champomy !
	if(current_button && !previous_button)
	ret = true;
	//on mémorise l'état actuel pour le prochain passage
	previous_button = current_button;
	return ret;
}

bool_e left_button_press_event(void)
{
	static bool_e previous_button = false;
	bool_e ret = false;
	bool_e current_button;
	current_button = !HAL_GPIO_ReadPin(LEFT_BUTTON_GPIO, LEFT_BUTTON_PIN);
	if(current_button && !previous_button)
	ret = true;
	previous_button = current_button;
	return ret;
}

bool_e right_button_press_event(void)
{
	static bool_e previous_button = false;
	bool_e ret = false;
	bool_e current_button;
	current_button = !HAL_GPIO_ReadPin(RIGHT_BUTTON_GPIO, RIGHT_BUTTON_PIN);
	if(current_button && !previous_button)
	ret = true;
	previous_button = current_button;
	return ret;
}

/*
 * @brief  Fonction qui sert de compteur pour un d�lai
 * @pre	  "Systick_add_callback_function(&process_ms)" doit �tre appel�e dans le main avant le while(1) si l'on souhaite que cette fonction soit appelee toutes les ms
 * @info   Le timer d'affichage agit sur la fréquence du buzzer et de la led... (pourquoi ?)
 */
uint16_t process_ms(void){
	if(!timer_extit) //Compteur pour les it externes
	{
		timer_extit = 2000;
		FLAG_IT = TRUE;
	}
	timer_extit--;
	if(VOTE_MODE_get_vote_mode_display_timer()==0) //Compteur pour l'affichage du mode vote
	{
		VOTE_MODE_set_vote_mode_display_timer(5000);
		VOTE_MODE_set_VOTE_MODE_DISPLAY_FLAG(true);
	}
	VOTE_MODE_vote_mode_dec_display_timer();
	return 0;
}
