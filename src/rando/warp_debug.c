/*************************************************************************
*   Copyright (C) 2020 Michael (Mittenz) Salino-Hugg
*   Contact: Twitter: @MittenzPhD
**************************************************************************
*	warp_debug.c
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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "warps.h"

int main(int argc, char *argv[]){
    unsigned int seed = 0;
    if(argc > 1){
        int seed = atoi(argv[1]);
        printf("Seed: 0x%08x\n", seed);
        printf("Test Include: 0x%02x\n", tree_available_init[0].me.map);
        wm_generate_mapping(seed);
    }
    else{
        printf("No Seed Provided\n");
        return 0;
    }
    return 0;
}
