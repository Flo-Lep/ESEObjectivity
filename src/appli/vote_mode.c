#include <stdio.h>
#include "vote_mode.h"
#include "nfc_sensor.h"
#include "flash_memory_management.h"
#include "stm32f1_ili9341.h"
#include "display.h"
#include "percentage.h"
#include "appli.h"

typedef enum{
	VOTE_INIT,
	READ_NFC_SENSOR,
	AUTHENTIFICATION,
	VOTE_INCREMENTATION,
	CONFIRMATION,
}vote_mode_e;

/*-----------------------------------------PRIVATE VARIABLES-------------------------------------*/
static bool vote_mode_enabled;
static vote_mode_e current_vote_mode;
static bool entrance;
static bool VOTE_MODE_DISPLAY_FLAG;
static volatile uint32_t vote_mode_display_timer;
/*------------------------------------PRIVATE FUNCTIONS PROTOTYPES-------------------------------*/
static void VOTE_MODE_init(void);
/*-----------------------------------------------------------------------------------------------*/

void VOTE_MODE_init(void){
	entrance = true;
	current_vote_mode = VOTE_INIT;
	VOTE_MODE_DISPLAY_FLAG = false;
	vote_mode_display_timer = 0;
}

void VOTE_MODE_management(void){
	static vote_mode_e vote_mode = VOTE_INIT;
	switch(vote_mode){
		case VOTE_INIT :
			VOTE_MODE_init();
			vote_mode = READ_NFC_SENSOR;
			current_vote_mode = vote_mode;
			break;
		case READ_NFC_SENSOR :
			NFC_SENSOR_init(); //Bricolage mais ça marche
			if(NFC_SENSOR_new_uid_available()){
				HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, true);
				vote_mode = AUTHENTIFICATION;
				current_vote_mode = vote_mode;
				entrance = true;
				DISPLAY_init(); //Bricolage mais ça marche
			}
			break;
		case AUTHENTIFICATION :
			if(VOTE_MODE_DISPLAY_FLAG){
				HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, false);
				if(FLASH_MEMORY_MANAGEMENT_is_uid_active(NFC_SENSOR_get_new_uid())){
					vote_mode = VOTE_INCREMENTATION;
					current_vote_mode = vote_mode;
					entrance = true;
				}
				else{
					ILI9341_printf(75,70,&Font_11x18,ILI9341_COLOR_RED, ILI9341_COLOR_WHITE,"ACCES REFUSE");
					vote_mode = VOTE_INIT;
					current_vote_mode = vote_mode;
					HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, false);
					APPLI_set_go_back_to_home_screen(true);
				}
				VOTE_MODE_DISPLAY_FLAG = false;
			}

			break;
		case VOTE_INCREMENTATION :
				if(DISPLAY_get_choice_made()){
					if(VOTE_MODE_DISPLAY_FLAG){
						vote_mode = CONFIRMATION;
						current_vote_mode = vote_mode;
						DISPLAY_set_choice_made(false);
						entrance = true;
						VOTE_MODE_DISPLAY_FLAG = false;
					}
				}
				else{
					entrance = true;
				}
			break;
		case CONFIRMATION :
			if(VOTE_MODE_DISPLAY_FLAG){
				FLASH_MEMORY_MANAGEMENT_block_uid_vote_from_flash(NFC_SENSOR_get_new_uid());
				APPLI_set_go_back_to_home_screen(true);
				vote_mode = VOTE_INIT;
				VOTE_MODE_DISPLAY_FLAG = false;
			}
			break;
		default :
			ILI9341_printf(50,50,&Font_7x10,ILI9341_COLOR_RED, ILI9341_COLOR_WHITE,"ERROR MANAGING VOTE MODES");
			break;
	}
}

void VOTE_MODE_display(void){
	if(entrance){
		switch(current_vote_mode){
		case VOTE_INIT:
			//
			break;
		case READ_NFC_SENSOR :
			ILI9341_Fill(ILI9341_COLOR_WHITE);
			ILI9341_printf(35,50,&Font_7x10,ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE,"PASSEZ VOTRE CARTE DEVANT LE LECTEUR");
			break;
		case AUTHENTIFICATION :
			ILI9341_Fill(ILI9341_COLOR_WHITE);
			ILI9341_printf(50,50,&Font_7x10,ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE,"AUTHENTIFICATION EN COURS...");
			break;
		case VOTE_INCREMENTATION :
			DISPLAY_2_choices(); //On fait sélectionner le candidat à incrémenter
			break;
		case CONFIRMATION :
			ILI9341_Fill(ILI9341_COLOR_WHITE);
			ILI9341_printf(35,80,&Font_7x10,ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE,"VOTRE VOTE A BIEN ETE PRIS EN COMPTE");
			break;
		default :
			break;
		}
		entrance = false;
	}
}

bool VOTE_MODE_get_status(void){
	return vote_mode_enabled;
}

void VOTE_MODE_set_status(bool status){
	if(status)
		vote_mode_enabled = true;
	else
		vote_mode_enabled = false;
}

void VOTE_MODE_set_VOTE_MODE_DISPLAY_FLAG(bool status){
	if(status)
		VOTE_MODE_DISPLAY_FLAG = true;
	else
		VOTE_MODE_DISPLAY_FLAG = false;
}

void VOTE_MODE_set_vote_mode_display_timer(uint32_t value){
	vote_mode_display_timer = value;
}

uint32_t VOTE_MODE_get_vote_mode_display_timer(){
	return vote_mode_display_timer;
}

void VOTE_MODE_vote_mode_dec_display_timer(){
	vote_mode_display_timer--;
}
