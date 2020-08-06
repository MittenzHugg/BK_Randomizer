/*************************************************************************
*   Copyright (C) 2020 Michael (Mittenz) Salino-Hugg
*   Contact: Twitter: @MittenzPhD
**************************************************************************
*	rando.h
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

#ifndef _RANDO_H
#define _RANDO_H

#include <list/list.h>
#include <libundermine.h>

typedef enum rando_mode{
    RANDO_MODE_SHORT = 1,
    RANDO_MODE_NORMAL = 2,
    RANDO_MODE_LONG = 3,
} rando_mode_t;

typedef struct {
    _Bool       ready;
    menu_t      main_menu;
    menu_t      pause_menu;
    int         menu_active;
    char       *tooltip;
    struct list watches;
    uint32_t    current_file;
    uint32_t    current_seed;
    uint32_t    seed[4];
    rando_mode_t mode[4];
    menu_item_t* seed_num;
    char       *cwd_name;
} rando_ctxt_t;



extern rando_ctxt_t rando;

void warp_interceptor(uint32_t map_indx, uint32_t exit_indx, uint32_t transition_type);
void warp_map_flag_clear_interceptor(uint32_t map_indx);
void rando_savestates_init(void);
void rando_warps_init(void);
#endif
