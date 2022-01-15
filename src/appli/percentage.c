#include "tft_ili9341/stm32f1_ili9341.h"
#include "display.h"
#include <math.h>

/*-----------------------------------------PRIVATE VARIABLES-------------------------------------*/
static uint8_t vote_1;
static uint8_t vote_2;
static uint8_t total_vote;
static float percentage_vote_1;
static float percentage_vote_2;
static uint8_t percentage_1;
static uint8_t percentage_2;
/*------------------------------------PRIVATE FUNCTIONS PROTOTYPES-------------------------------*/
static void PERCENTAGE_compute_total_vote(void); //permet de calculer le nombre de voix au total
static void PERCENTAGE_compute_1(void); //permet de calculer le pourcentage de voix pour le choix 1
static void PERCENTAGE_compute_2(void); //permet de calculer le pourcentage de voix pour le choix 2
/*-----------------------------------------------------------------------------------------------*/
void PERCENTAGE_init(void){
	vote_1 = 0;
	vote_2 = 0;
	total_vote = 0;
	percentage_vote_1 = 0;
	percentage_vote_2 = 0;
}

void PERCENTAGE_compute_total_vote(void){
	total_vote = vote_1 + vote_2;
}

void PERCENTAGE_compute_1(void){
	percentage_vote_1 = ((float)(vote_1*100)/total_vote);
}

void PERCENTAGE_compute_2(void){
	percentage_vote_2 = ((float)(vote_2*100)/total_vote);
}

void PERCENTAGE_compute_all(void){
	PERCENTAGE_compute_total_vote();
	PERCENTAGE_compute_1();
	PERCENTAGE_compute_2();
	percentage_1 = roundf(percentage_vote_1);
	percentage_2 = roundf(percentage_vote_2);
	}

uint16_t PERCENTAGE_graph(void){
	if(percentage_1 == 100){
		return 290; //Douille internationale
	}
	else{
		return ((percentage_vote_1*320)/100);
	}
}

void PERCENTAGE_add_vote_1(void){
	vote_1++;
}

void PERCENTAGE_add_vote_2(void){
	vote_2++;
}

/*ACCESSEURS*/

uint8_t PERCENTAGE_get_percentage_1(void){
	return percentage_1;
}

uint8_t PERCENTAGE_get_percentage_2(void){
	return percentage_2;
}

uint8_t PERCENTAGE_get_total_vote(void){
	return total_vote;
}
