#include <stdint.h>
#include "bk.h"


void rando_restore_items(void){
    bk_restore_health();
    //ToDo: restore note counts to high note score;
    //ToDo: restore jinjo counts;
}

void rando_savestates_save(uint32_t map_indx){
    //ToDo: find where to store half of these in the expansion
    bk_map_savestate_save(map_indx);
}

void rando_savestates_init(void){
    //prevent map savestate clear
    bk_map_savestate_clear_all_hook1 = 0x00000000;//nop
    osInvalICache((void*)&bk_map_savestate_clear_all_hook1, 4);
    bk_map_savestate_clear_hook = 0x00000000;//nop
    osInvalICache((void*)&bk_map_savestate_clear_hook, 4);


    //savestate saving
    uint32_t ss_save_p = (uint32_t)&rando_savestates_save;
    ss_save_p = ((ss_save_p & 0xFFFFFF) >> 2 ) | 0xC000000;
    bk_map_savestate_save_hook = ss_save_p;
    osInvalICache((void*)&bk_map_savestate_save_hook, 4);

    //item reload on level enter
    uint32_t reload_p = (uint32_t)&rando_restore_items;
    reload_p = ((reload_p & 0xFFFFFF) >> 2 ) | 0xC000000;
    bk_restore_health_hook = reload_p;
    osInvalICache((void*)&bk_restore_health_hook, 4);
}