#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f1_flash.h"
#include "flash_memory_management.h"

typedef struct{
	uint32_t uid_low_part;
	uint32_t uid_high_part;
	uint8_t index;
}uid64_t;

/*-----------------------------------------PRIVATE VARIABLES-------------------------------------*/
static uid64_t full_uid;
static uint16_t flash_index;//De 0 à 255 : numéro de la case de la flash

/*------------------------------------PRIVATE FUNCTIONS PROTOTYPES-------------------------------*/
static void FLASH_MEMORY_MANAGEMENT_split_uid(uint64_t uid_to_split);
static void FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(uint64_t uid);
static void FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(uint64_t uid);
static void FLASH_MEMORY_MANAGEMENT_disable_uid_from_flash(uint64_t uid);
/*-----------------------------------------------------------------------------------------------*/

/*------------------------------------------FEATURES---------------------------------------------*/

void FLASH_MEMORY_MANAGEMENT_init(void){
	flash_index = 0;
	//Faut ptetre erase la flash avant ?
	//On intègre les uid dans la flash et on les active
	//LEOPOLD
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x045947da7a578000);
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x045947da7a578000);
	//FLO
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x047d50f208538000);
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x047d50f208538000);
	//QUENTIN
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x043a183af9628000);
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x043a183af9628000);
	//GENS DE LA CLASSE
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x045e30baad588000);
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x045e30baad588000);
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x042c58aa7a578000);
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x042c58aa7a578000);
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x0488200209538000);
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x0488200209538000);
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x04636dd2495a8000);
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x04636dd2495a8000);
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x04965df208538000);
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x04965df208538000);
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x042c4092ef548000);
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x042c4092ef548000);
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x04484a5aa8558000);
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x04484a5aa8558000);
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x047b41a27a578000);
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x047b41a27a578000);
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x042f1faa7a578000);
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x042f1faa7a578000);
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x04185cf208538000);
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x04185cf208538000);
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x04125cf208538000);
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x04125cf208538000);
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x048f2592ef548000);
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x048f2592ef548000);
	//Bon faudrait pas qu'en y'en ait plus...
}

void FLASH_MEMORY_MANAGEMENT_split_uid(uint64_t uid_to_split){
	full_uid.uid_high_part = (uint32_t)((uid_to_split)&0xFFFFFFFF);
	full_uid.uid_low_part = (uint32_t)((uid_to_split>>32)&0xFFFFFFFF);
}

void FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(uint64_t uid){
	if(flash_index == 253)
		printf("Mémoire pleine, impossible d'ajouter un nouvel uid\n");
	else{
		FLASH_MEMORY_MANAGEMENT_split_uid(uid);
		FLASH_set_word(flash_index,full_uid.uid_low_part);
		flash_index++;
		FLASH_set_word(flash_index,full_uid.uid_high_part);
		flash_index++;
	}
}

void FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(uint64_t uid){
	if(FLASH_MEMORY_MANAGEMENT_find_uid(uid))
		FLASH_set_word((full_uid.index+1),(full_uid.uid_high_part+1)); //Le dernier octet vaut 1

}

void FLASH_MEMORY_MANAGEMENT_disable_uid_from_flash(uint64_t uid){
	if(FLASH_MEMORY_MANAGEMENT_find_uid(uid)){
		FLASH_set_word((full_uid.index+1),(full_uid.uid_high_part-1)); //Le dernier octet vaut 0
	}
}

void FLASH_MEMORY_MANAGEMENT_block_uid_vote_from_flash(uint64_t uid){ //Si le gars a déjà voté
	if(FLASH_MEMORY_MANAGEMENT_is_uid_active(uid)){
			FLASH_set_word((full_uid.index+1),(full_uid.uid_high_part+1)); //On passe le dernier octet à 2
		}
}

bool FLASH_MEMORY_MANAGEMENT_is_uid_active(uint64_t uid_to_check){
	bool ret_ = false;
	FLASH_MEMORY_MANAGEMENT_split_uid(uid_to_check);
	full_uid.uid_high_part++; //On fait l'hypothèse que l'uid est actif (puisqu'on a appelé la fonction...)
	for(uint8_t i=0;i<flash_index;i++){
		if(FLASH_read_word(i)==full_uid.uid_low_part && FLASH_read_word(i+1)==full_uid.uid_high_part){
			full_uid.index = i;
			ret_ = true;
			break;
		}
		i++;
	}
	return ret_;
}

bool FLASH_MEMORY_MANAGEMENT_find_uid(uint64_t uid_to_find){
	FLASH_MEMORY_MANAGEMENT_split_uid(uid_to_find);
	//uid non actif
	for(uint8_t i=0;i<flash_index;i++){
				if(FLASH_read_word(i)==full_uid.uid_low_part && FLASH_read_word(i+1)==full_uid.uid_high_part){
					full_uid.index = i;
					return true;
					break;
				}
				i++;
	}
	//uid actif
	full_uid.uid_high_part++; //On fait l'hypothèse que l'uid est actif
	for(uint8_t i=0;i<flash_index;i++){
		if(FLASH_read_word(i)==full_uid.uid_low_part && FLASH_read_word(i+1)==full_uid.uid_high_part){
			full_uid.index = i;
			return true;
			break;
		}
		i++;
	}
	//uid bloqué -> Vote déjà effectué
	full_uid.uid_high_part++; //On fait l'hypothèse que l'uid est bloqué
	for(uint8_t i=0;i<flash_index;i++){
		if(FLASH_read_word(i)==full_uid.uid_low_part && FLASH_read_word(i+1)==full_uid.uid_high_part){
			full_uid.index = i;
			return true;
			break;
		}
		i++;
	}
	printf("ERROR : uid not found\n");
	return false;
}


/**********************************TEST FUNCTIONS******************************/
void FLASH_MEMORY_MANAGEMENT_display_flash(void)
{
	uint32_t index = flash_index;
	uint32_t v;
	printf("Affichage des %d donn�es (32 bits) disponibles dans le dernier secteur de la FLASH\n", flash_index);
	for(index = 0; index<flash_index; index++)
	{
		v = FLASH_read_word(index);
		printf("@%03ld : 0x%08lx = %ld\n", index, v, v);
	}
}

void FLASH_MEMORY_MANAGEMENT_test(void){
	FLASH_MEMORY_MANAGEMENT_add_new_uid_in_flash(0x045947da7a578000);
	FLASH_MEMORY_MANAGEMENT_display_flash();
	FLASH_MEMORY_MANAGEMENT_enable_uid_from_flash(0x045947da7a578000);
	FLASH_MEMORY_MANAGEMENT_display_flash();
	//FLASH_MEMORY_MANAGEMENT_disable_uid_from_flash(0x045947da7a578000);
	//FLASH_MEMORY_MANAGEMENT_display_flash();
	if(FLASH_MEMORY_MANAGEMENT_is_uid_active(0x045947da7a578000))
		printf("UID ACTIVE");
	else
		printf("UID DISABLED");
}
