#include <stdint.h>
#include "bk.h"
#include "sys.h"

//intercepts takeMeThere
void warp_interceptor(uint32_t map_indx, uint32_t exit_indx, uint32_t transition_type){
    uint32_t rando_map_indx = map_indx;
    uint32_t rando_exit_indx = exit_indx;
    
    // TODO look up shuffled map_exit pair from generated lookup table;

    bk_map_exit_no_reset_set(rando_map_indx, rando_exit_indx, transition_type);
    return;
}

void warp_generate_mapping(void){
    //TODO generate Loadzone Mapping
    return;
}

void warp_clear_saveState_interceptor(void){
    
    //todo save saveStates to ed and reload valid chunk based on next room

    //bk_map_savestate_clear_all(); //TODO remove 
    return;
}
