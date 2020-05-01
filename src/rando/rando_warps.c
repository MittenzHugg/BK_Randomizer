#include <stdint.h>
#include "bk.h"
#include "sys.h"
#include "warps.h"
#include "simptypes.h"

typedef struct exitMap{
    exit_t old;
    exit_t new;
} exitMap_t;

const u16 hash_table_len = 337; //about 1.3x exit count

static exitMap_t exitLUT[337] = {0};

bool warp_exits_match(exit_t* a, exit_t* b){
    return ((a->map == b->map) && (a->exit == b->exit));
}

u16 warp_exit_hash(exit_t* a){
    u16 hash_seed = (((u16) a->exit) << 8) | ((u16)a->map);
    return hash_seed%hash_table_len;
}

exitMap_t* exitLUT_get(exit_t* old){
    exitMap_t* ret_val;
    u16 i = warp_exit_hash(old);
    do{
        ret_val = &exitLUT[i];
        if(ret_val->old.map == 0)
            return NULL;
        i++;
    }while (!warp_exits_match(old, &(ret_val->old)));
    return ret_val;
}

void exitLUT_set(exit_t* old, exit_t* new){
    exitMap_t* ret_val;
    u16 i = warp_exit_hash(old);
    do{
        ret_val = &exitLUT[i];
         if(ret_val->old.map == 0){
            ret_val->old = *old;
            ret_val->new = *new;
            return;
         }
            
        i++;
    }while (true);
}

void exitLUT_clear(void){
    exitMap_t null_exit = {{0,0}, {0,0}};
    for(int i = 0; i<hash_table_len; i++){
        exitLUT[i] = null_exit;
    }
}

void warp_interceptor(uint32_t old_map, uint32_t old_exit, uint32_t transition_type){
    uint32_t new_map =  old_map;
    uint32_t new_exit =  old_exit;
    exit_t old = {(u8) old_map, (u8) old_exit};

    exitMap_t* mapping = exitLUT_get(&old);
    if(mapping != NULL){
        new_map = (uint32_t) mapping->new.map;
        new_exit = (uint32_t) mapping->new.exit;
    }

    bk_map_exit_no_reset_set(new_map, new_exit, transition_type);
    return;
}

void warp_generate_mapping(void){
    exitLUT_clear();
    // reinit random seed

    // TODO generate Loadzone Mapping
    exit_t test_old = {0x01,0x13}; //exit gr_lair to sm
    exit_t test_new = {0x43,0x08}; //warps to ccw_spring_top
    exitLUT_set(&test_old, &test_new);


    test_new = (exit_t) {0x69,0x12}; //exit to ccw_spring to top_room 
    test_old = (exit_t) {0x65,0x01}; //warps to gr_lair
    exitLUT_set(&test_old, &test_new);

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




