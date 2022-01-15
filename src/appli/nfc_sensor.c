
#include <stdbool.h>
#include "NFC03A1/nfc03a1.h"
#include "nfc_sensor.h"




/*-----------------------------------------PRIVATE VARIABLES-------------------------------------*/
static uint64_t new_uid_read;
/*-----------------------------------------------------------------------------------------------*/

void NFC_SENSOR_init(void){
	NFC03A1_Init(PCD);
}

//Cette fonction renvoie vrai dès lors qu'un nouvel uid est lu et le stocke dans new_uid_read
bool NFC_SENSOR_new_uid_available(void){
	bool ret = FALSE;
	uint8_t tag;
	tag = ConfigManager_TagHunting(TRACK_ALL);
	switch(tag)
	{
		case TRACK_NFCTYPE4A:{
			ISO14443A_CARD infos;
			NFC03A1_get_ISO14443A_infos(&infos);
			uint8_t i;
			new_uid_read = 0;
			for(i=0; i<infos.UIDsize;i++){
				new_uid_read+= ((uint64_t)infos.UID[i])<<((7-i)*8); //On stocke le nouvel uid lu dans un entier
			}
			ret = true;
			break;}
		default:
			break;
	}
	return ret;
}


uint64_t NFC_SENSOR_get_new_uid(void){
	return new_uid_read;
}
