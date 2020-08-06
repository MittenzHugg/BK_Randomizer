/*************************************************************************
*   Copyright (C) 2020 Michael (Mittenz) Salino-Hugg
*   Contact: Twitter: @MittenzPhD
**************************************************************************
*	rando_warps.c
*
*   This file is part of BK_Randomizer.
*
*   BK_Randomizer is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   BK_Randomizer is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with BK_Randomizer.  If not, see <https://www.gnu.org/licenses/>.
*************************************************************************/

#include <stdint.h>
#include "bk.h"
#include "sys.h"
#include "warps.h"
#include "simptypes.h"
#include "rando.h"


void warp_interceptor(uint32_t old_map, uint32_t old_exit, uint32_t transition_type){
    uint32_t new_map =  old_map;
    uint32_t new_exit =  old_exit;
    exit_t old = {(u8) old_map, (u8) old_exit};

    if (!bk_moves_unlocked_get(bk_moves_bearPunch)){
        bk_map_exit_no_reset_set(old_map, old_exit, transition_type);
        return;     
    }
    if(bk_cutscene_map){
        bk_map_exit_no_reset_set(old_map, old_exit, 1);
        return; 
    }
    if(old_map == bk_main_map && old_exit == bk_main_exit){
        bk_map_exit_no_reset_set(old_map, old_exit, 1);
        return; 
    }

    exitMap_t* mapping = exitLUT_get(&old);
    if(mapping != NULL){
        new_map = (uint32_t) mapping->new.map;
        new_exit = (uint32_t) mapping->new.exit;
    }

    bk_map_exit_no_reset_set(new_map, new_exit, 1);
    return;
}


void warp_generate_mapping(void){
    wm_generate_mapping(rando.current_seed);
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




