#include <stdint.h>
#include "bk.h"
#include "sys.h"

void warp_interceptor(uint32_t old_map, uint32_t old_exit, uint32_t transition_type){
    uint32_t new_map = old_map;
    uint32_t new_exit = old_exit;
    
    /* TODO 
       old map and exit -> new map and exit
       based on mapping generated in warp_generate_mapping();
    */
   //new_map = 0x01;
   //new_exit = 0x12;

    bk_map_exit_no_reset_set(new_map, new_exit, transition_type);
    return;
}

void warp_generate_mapping(void){
    // TODO generate Loadzone Mapping

    return;
}

void rando_warps_init(void){

    //warps hook (warp mapping)
    uint32_t warp_interceptor_p = (uint32_t)&warp_interceptor;
    bk_map_exit_no_reset_set_hook = ((warp_interceptor_p & 0xFFFFFF) >> 2) | 0xC000000;
    osInvalICache((void*)&bk_map_exit_no_reset_set_hook, 4);

    //return to last exit on voidout/deathwarp
    bk_void_to_map_logic = 0x10000004; //beq zero, zero
    osInvalICache((void*)&bk_void_to_map_logic, 4);
    
    //remove level reset on deathwarp
    uint32_t take_me_there_p = (uint32_t)&bk_take_me_there;
    bk_deathwarp_take_me_there_hook =((take_me_there_p & 0xFFFFFF) >> 2) | 0xC000000;
    osInvalICache((void*)&bk_deathwarp_take_me_there_hook, 4);

    return;
}




