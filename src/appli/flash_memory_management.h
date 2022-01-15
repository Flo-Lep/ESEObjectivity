#ifndef FLASH_MEMORY_MANAGEMENT_H
#define FLASH_MEMORY_MANAGEMENT_H

/*PUBLIC FUNCTIONS PROTOTYPES*/
void FLASH_MEMORY_MANAGEMENT_init(void);
bool FLASH_MEMORY_MANAGEMENT_find_uid(uint64_t uid_to_find);
void FLASH_MEMORY_MANAGEMENT_block_uid_vote_from_flash(uint64_t uid);
bool FLASH_MEMORY_MANAGEMENT_is_uid_active(uint64_t uid_to_check);
void FLASH_MEMORY_MANAGEMENT_display_flash(void);
void FLASH_MEMORY_MANAGEMENT_test(void);

#endif
