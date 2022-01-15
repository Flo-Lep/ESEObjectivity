#ifndef PERCENTAGE_H
#define PERCENTAGE_H

void PERCENTAGE_init(void);
void PERCENTAGE_compute_all(void);
void PERCENTAGE_add_vote_1(void); //permet d'ajouter un vote au premier choix
void PERCENTAGE_add_vote_2(void); //permet d'ajouter un vote au deuxième choix
uint8_t PERCENTAGE_graph(void); //permet de calculer la taille du rectangle bleu pour l'affichage de la proportion de vote pour le choix 1
uint8_t PERCENTAGE_get_percentage_1(void);
uint8_t PERCENTAGE_get_percentage_2(void);
uint8_t PERCENTAGE_get_total_vote(void);

#endif // PERCENTAGE_H
