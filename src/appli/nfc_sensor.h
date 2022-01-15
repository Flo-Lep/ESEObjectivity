#include <stdbool.h>
#include "config.h"

#ifndef NFC_SENSOR_H
#define NFC_SENSOR_H


/*------PUBLIC FUNCTIONS PROTOTYPES-----*/
void NFC_SENSOR_init(void);
bool NFC_SENSOR_new_uid_available(void);
uint64_t NFC_SENSOR_get_new_uid(void);

#endif
