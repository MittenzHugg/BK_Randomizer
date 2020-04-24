#include <stdint.h>
#include "bk.h"
#include "sys.h"

void warp_generate_mapping(void){
    // TODO generate Loadzone Mapping

    return;
}

void warp_interceptor(uint32_t old_map, uint32_t old_exit, uint32_t transition_type){
    uint32_t new_map = map_indx;
    uint32_t new_exit = exit_indx;
    
    /* TODO 
       old map and exit -> new map and exit
       based on mapping generated in warp_generate_mapping();
    */

    bk_map_exit_no_reset_set(new_map, new_exit, transition_type);
    return;
}

void warp_clear_saveState_interceptor(void){
    
    //todo save saveStates to ed and reload valid chunk based on next room

    //bk_map_savestate_clear_all(); //TODO remove 
    return;
}
