/*************************************************************************
*   Copyright (C) 2020 Michael (Mittenz) Salino-Hugg
*   Contact: Twitter: @MittenzPhD
**************************************************************************
*	randoresource.c
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

#include "randoresource.h"

static const char *resource_names[R_RANDO_END] = {
    "font",
    "buttons",
};

static resource_ctor resource_ctors[R_RANDO_END] = {
    grc_ctor,
    grc_ctor,
};

int resource_handles[R_RANDO_END] = { -1 };

void rando_resource_init(){
    for(int i = 0;i < R_RANDO_END;i++){
        resource_ent_t *res = resource_create(resource_names[i], resource_ctors[i], NULL);
        if(res){
            resource_handles[i] = res->resource_id;
        }
    }
}