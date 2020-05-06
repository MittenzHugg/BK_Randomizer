#include "warps.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "tree.h"

#ifdef DEBUG
#define D_PRINTF(f_, ...) printf((f_), ##__VA_ARGS__)
#else
#define D_PRINTF(f_, ...) while(false)
#endif

const u16 hash_table_len = 337;
exitMap_t exitLUT[337] = {0};

tree_t* banjo_tree;

//u64  achieved_flags = 0;
warp_hard_flags_t achieved_hard_flags = {WF_HARD_NONE};
warp_soft_flags_t achieved_soft_flags = {WF_SOFT_NONE};
u32  tree_includes_map_flag = 0;
#define TREE_MAP_TTC_TTC        1 << 1
#define TREE_MAP_GV_GV          1 << 2
#define TREE_MAP_CCW_HUB        1 << 3
#define TREE_MAP_CCW_SPRING     1 << 4
#define TREE_MAP_CCW_SUMMER     1 << 5
#define TREE_MAP_CCW_AUTUMN     1 << 6
#define TREE_MAP_TTC_CC_PUZZLE  1 << 7
#define TREE_MAP_BGS_PUZZLE     1 << 8
#define TREE_MAP_FP_PUZZLE      1 << 9
#define TREE_MAP_MMM_RBB_PUZZLE 1 << 10
#define TREE_MAP_CCW_PUZZLE     1 << 11
#define TREE_MAP_DOG            1 << 12
#define TREE_MAP_GV_PUZZLE      1 << 13
#define TREE_MAP_WS1            1 << 14
#define TREE_MAP_WS2            1 << 15
#define TREE_MAP_MM_MUMBO       1 << 16
#define TREE_MAP_BGS_MUMBO      1 << 17
#define TREE_MAP_FP_MUMBO       1 << 18
#define TREE_MAP_MMM_MUMBO      1 << 19
#define TREE_MAP_CCW_MUMBO      1 << 20
#define TREE_MAP_TTC_BLUBBER    1 << 21
#define TREE_MAP_TTC_SANDCASTLE 1 << 22
#define TREE_MAP_CC_CC          1 << 23
#define TREE_MAP_CC_INSIDE      1 << 24

u8   tree_jiggy_cnt = 0;
u8   tree_token_cnt = 0;
u16  tree_note_cnt = 0;
warp_pool_t nodePool_a;
warp_pool_t nodePool_u;
warp_pool_t tree_a;
warp_pool_t tree_u;


u64 warp_lsfr = 0;
void warp_srand(const u32 seed){
    warp_lsfr = (u64) seed;
}

u32 warp_rand(void){
    u64 bit;
    //taps 16, 14, 13, 11
    bit = ((warp_lsfr >> 0) ^ (warp_lsfr >> 1) ^ (warp_lsfr >> 3) ^ (warp_lsfr >> 4)); 
    warp_lsfr = (warp_lsfr >> 1) | (bit << 63);
    return (u32)(warp_lsfr & 0x0FFFFFFFF);
}

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
        i= i%hash_table_len;
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
        i= i%hash_table_len;
    }while (true);
}

static map_e _warp_get_map_index(exit_t* warp){
    map_e imap = warp->map;
    if(imap == TTC_SHIP){
        if(warp->exit == 0x05){
            imap = TTC_SHIP_TOP;
        } else if(warp->exit == 0x06){
            imap = TTC_SHIP_BOTTOM;
        }
    } else if (imap == RBB_ENGINE){
        if(warp->exit == 0x01){
            imap = RBB_ENGINE_GEARS;
        } else if(warp->exit == 0x04 ){
            imap = RBB_ENGINE_BUTTON;
        }
    }
    return imap;
}

void exitLUT_clear(void){
    exitMap_t null_exit = {{0,0}, {0,0}};
    for(int i = 0; i<hash_table_len; i++){
        exitLUT[i] = null_exit;
    }
}

warp_t nodePool_available_init[] ={
    //MM,mm
    {{0x02,0x01}, {0x0E,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MM,mm <- MM,mm_mumbos
    {{0x02,0x02}, {0x0C,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MM,mm <- MM,mm_tickers_lower
    {{0x02,0x04}, {0x69,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MM,mm <- lair,mm_lobby

    //TTC,blubber_ship
    {{0x05, 0x05}, {0x07,0x06}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //TTC,blubber_ship_top <- TTC,ttc
    {{0x05, 0x06}, {0x07,0x07}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //TTC,blubber_ship_bottom <- TTC,ttc
    
    //TTC,nipper_shell
    {{0x06, 0x01}, {0x07,0x0A}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //TTC, nipper_shell <- TTC,ttc

    //TTC, ttc
    {{0x07,0x03}, {0x0A, 0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //TTC,ttc <- TTC,sandcastle
    {{0x07,0x04}, {0x6D, 0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //TTC,ttc <- lair,ttc_lobby
    {{0x07,0x07}, {0x05, 0x06}, {WF_HARD_NONE}, {WF_SOFT_NONE}},      //TTC,ttc <- TTC,blubber_ship_bottom
    {{0x07,0x0A}, {0x06, 0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //TTC,ttc <- TTC,nipper_shell

    //TTC, sandcastle
    {{0x0A, 0x01}, {0x07,0x03}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //TTC,sandcastle <- TTC,ttc

    //CC, cc
    {{0x0B, 0x03}, {0x22,0x05}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CC,cc <- CC,inside_left_gil
    {{0x0B, 0x04}, {0x22,0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CC,cc <- CC,inside_right_gil
    {{0x0B, 0x05}, {0x70,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CC,cc <- lair,cc_lobby

    //MM,mm_tickers
    {{0x0C,0x02}, {0x02,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}},     //MM,mm_tickers_lower <- MM,mm 

    //BGS, bgs
    {{0x0D,0x02}, {0x72,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}},     //BGS,bgs <- lair,bgs_lobby

    //MM,mm_mumbos
    {{0x0E,0x01}, {0x02,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MM,mm_mumbos <- MM,mm

    //BGS,tanktup
    {{0x11,0x01}, {0x0D,0x03}, {WF_HARD_NONE}, {WF_SOFT_NONE}},     //BGS,tanktup <- BGS,bgs  

    //GV,gv
    {{0x12,0x08}, {0x6E,0x03}, {WF_HARD_NONE}, {WF_SOFT_NONE}},      //GV,gv <- lair, GV_Lobby

    //GV,matching_game
    {{0x13,0x01}, {0x12,0x03}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //GV,matching_game <- GV,gv 

    //GV,sandybutt
    {{0x14,0x01}, {0x12,0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //sandybutt_front <- GV,gv 

    //GV,water_pyramid
    {{0x15,0x06}, {0x12,0x05}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //GV,water_pyramid_front <- GV,gv

    //GV,rubee
    {{0x16,0x07}, {0x12,0x06},  {WF_HARD_NONE}, {WF_SOFT_NONE}}, //GV,rubee <- GV,gv

    //GV,jinxy
    {{0x1A,0x01}, {0x12,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //GV,jinxy <- GV,gv

    //MMM,mmm
    {{0x1B,0x01}, {0x26,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm <- MMM,dining_room_front
    {{0x1B,0x03}, {0x25,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm <- MMM,well_top
    {{0x1B,0x04}, {0x24,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm <- MMM,tumblar
    {{0x1B,0x06}, {0x2B,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm <- MMM,church_secret
    {{0x1B,0x09}, {0x1D,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm <- MMM,cellar
    {{0x1B,0x0A}, {0x2A,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm <- MMM,red_feather_room
    {{0x1B,0x0B}, {0x28,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm <- MMM,blue_egg_room
    {{0x1B,0x0C}, {0x2C,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm <- MMM,bathroom
    {{0x1B,0x0D}, {0x2E,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm <- MMM,honey_carrier_room
    {{0x1B,0x0E}, {0x2D,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm <- MMM,bedroom
    {{0x1B,0x0F}, {0x29,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm <- MMM,music_note_room
    {{0x1B,0x10}, {0x1B,0x11}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm_clock_bottom <- MMM,mmm_clock_top
    {{0x1B,0x12}, {0x30,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm <- MMM,mmm_mumbos
    {{0x1B,0x14}, {0x75,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm <- lair,mmm_lobby

    //MMM,church
    {{0x1C,0x01}, {0x1B,0x05}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,church <- MMM,mmm

    //MMM, cellar
    {{0x1D,0x01}, {0x1B,0x09}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,cellar <- MMM,mmm

    //CC, inside
    {{0x22,0x04}, {0x0B,0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}},         //CC,inside_right_gil       <- CC,cc
    {{0x22,0x05}, {0x0B,0x03}, {WF_HARD_NONE}, {WF_SOFT_NONE}},         //CC,inside_left_gil       <- CC,cc

    //CC,wonderwing
    {{0x23,0x01}, {0x22,0x03}, {WF_HARD_NONE}, {WF_SOFT_NONE}},  //CC,wonderwing <- CC,inside

    //MMM,tumblar
    {{0x24,0x01}, {0x1B,0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,tumblar <- MMM,mmm 

    //MMM,well_top
    {{0x25,0x01}, {0x1B,0x03}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,well_top <- MMM,mmm

    //MMM,dining_room
    {{0x26,0x01}, {0x1B,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}},     //MMM,dining_room_front <- MMM,mmm 

    //FP,fp
    {{0x27,0x07}, {0x48,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //FP,fp <- FP,mumbos
    {{0x27,0x09}, {0x53,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //FP,fp <- FP,tree 

    //MMM,blue_egg_room
    {{0x28,0x01}, {0x1B,0x0B}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,blue_egg_room <- MMM,mmm

    //MMM,music_note_room
    {{0x29,0x01}, {0x1B,0x0F}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,music_note_room <- MMM,mmm

    //MMM,red_feather_room
    {{0x2A,0x01}, {0x1B,0x0A}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,red_feather_room <- MMM,mmm 

    //MMM,chruch_secret
    {{0x2B,0x01}, {0x1B,0x06}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,church_secret <- MMM,mmm

    //MMM,bathroom
    {{0x2C,0x01}, {0x1B,0x0C}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,bathroom <- MMM,mmm

    //MMM,bedroom
    {{0x2D,0x01}, {0x1B,0x0E}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,bedroom <- MMM,mmm

    //MMM,honey_carrier_room
    {{0x2E,0x01}, {0x1B,0x0D}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,honey_carrier_room <- MMM,mmm

    {{0x2F,0x02}, {0x1B,0x06}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm <- MMM,rain_barrel

    //MMM,mmm_mumbos
    {{0x30,0x01}, {0x1B,0x12}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,mmm_mumbos <- MMM,mmm

    //RBB, rbb
    {{0x31,0x01}, {0x3F,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- RBB,capt_quarters
    {{0x31,0x02}, {0x39,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- RBB,crew_quarters
    {{0x31,0x03}, {0x34,0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- RBB,engine_button
    {{0x31,0x04}, {0x3C,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- RBB,kitchen
    {{0x31,0x05}, {0x3D,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- RBB,navigation
    {{0x31,0x06}, {0x3B,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- RBB,hull
    {{0x31,0x07}, {0x34,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- RBB,engine_gears
    {{0x31,0x08}, {0x35,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- RBB,boat_house
    {{0x31,0x09}, {0x37,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- RBB,left_container
    {{0x31,0x0A}, {0x3E,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- RBB,middle_container
    {{0x31,0x0B}, {0x38,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- RBB,right_container
    {{0x31,0x0C}, {0x3A,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- RBB,boss_boombox
    {{0x31,0x0D}, {0x36,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- RBB,warehouse_front
    {{0x31,0x10}, {0x77,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- lair,rbb_lobby
    {{0x31,0x13}, {0x8B,0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- RBB,anchor_room

    //RBB,engine
    {{0x34,0x01}, {0x31,0x07}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,engine_gears <- RBB,rbb
    {{0x34,0x04}, {0x31,0x03}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,engine_button <- RBB,rbb 

    //RBB,boat_house
    {{0x35,0x01}, {0x31,0x08}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,boat_house <- RBB,rbb 

    //RBB,warehouse
    {{0x36,0x01}, {0x31,0x0d}, {WF_HARD_NONE}, {WF_SOFT_NONE}},   //RBB,warehouse_front <- RBB,rbb

    //RBB,left_container
    {{0x37,0x01}, {0x31,0x09}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,left_container <- RBB,rbb 

    //RBB, right_container
    {{0x38,0x01}, {0x31,0x0B}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,right_container <- RBB,rbb

    //RBB,crew_quarters
    {{0x39,0x01}, {0x31,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,crew_quarters <- RBB,rbb
    
    //RBB,boss_boombox
    {{0x3A,0x01}, {0x31,0x0C}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,boss_boombox <- RBB,rbb

    //RBB,hull
    {{0x3B,0x01}, {0x31,0x06}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,hull <- RBB,rbb 

    //RBB,kitchen
    {{0x3C,0x01}, {0x31,0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,kitchen <- RBB,rbb

    //RBB navigation
    {{0x3D,0x01}, {0x31,0x05}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,navigation <- RBB,rbb 

    //RBB,middle_container
    {{0x3E,0x01}, {0x31,0x0A}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,rbb <- RBB,middle_container

    //RBB,capt_quarters
    {{0x3F,0x01}, {0x31,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,capt_quarters <- RBB,rbb

    //CCW,hub
    {{0x40,0x07}, {0x79,0x06}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,hub <- lair,ccw_lobby

    //FP,igloo
    {{0x41,0x01}, {0x27,0x08}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //FP,igloo <- FP,fp

    //CCW_Spring
    {{0x43,0x01}, {0x40,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,spring <- CCW,hub
    {{0x43,0x09}, {0x4A,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,spring <- CCW,sp_mumbos

     //CCW_Summer
    {{0x44,0x01}, {0x40,0x03}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,summer <- CCW,hub
    {{0x44,0x09}, {0x4B,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,spring <- CCW,su_mumbos

    //CCW_AUTUMN
    {{0x45,0x01}, {0x40,0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,autumn <- CCW,hub
    {{0x45,0x09}, {0x4C,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,autumn <- CCW,au_mumbos

    //CCW,winter
    {{0x46,0x01}, {0x40,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,winter <- CCW,hub
    {{0x46,0x09}, {0x4D,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,autumn <- CCW,au_mumbos

    //BGS, bgs_mumbos
    {{0x47,0x01}, {0x0D,0x06}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //BGS,bgs_mumbos <- BGS,bgs

    //FP, fp_mumbos
    {{0x48,0x01}, {0x27,0x07}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //FP,fp <- FP,mumbos

    //CCW,sp_mumbos
    {{0x4A,0x01}, {0x43,0x09}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,spring <- CCW,sp_mumbos
    //CCW,su_mumbos
    {{0x4B,0x01}, {0x44,0x09}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,spring <- CCW,su_mumbos
    //CCW,au_mumbos
    {{0x4C,0x01}, {0x45,0x09}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,spring <- CCW,au_mumbos
    //CCW,wi_mumbos
    {{0x4D,0x01}, {0x46,0x09}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,spring <- CCW,au_mumbos

    //FP,tree
    {{0x53,0x01}, {0x27,0x09}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //FP,tree <- FP,fp

    //CCW,zubbas
    {{0x5B,0x02}, {0x44,0x06}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,su_zubbas <- CCW,summmer
    {{0x5C,0x01}, {0x45,0x05}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,au_zubbas <- CCW,autumn

    //CCW,sp_nabuts
    {{0x5E,0x01}, {0x43,0x07}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,door <- CCW,spring
    {{0x5E,0x02}, {0x43,0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,window <- CCW,spring
    //CCW,su_nabuts
    {{0x5F,0x01}, {0x44,0x07}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,door <- CCW,summer
    {{0x5F,0x02}, {0x44,0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,window <- CCW,summer
    //CCW,au_nabuts
    {{0x60,0x01}, {0x45,0x07}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,door <- CCW,summer
    {{0x60,0x02}, {0x45,0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,window <- CCW,summer
    //CCW,wi_nabuts
    {{0x61,0x01}, {0x46,0x07}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,door <- CCW,summer
    {{0x61,0x02}, {0x46,0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,window <- CCW,summer

    //CCW,wi_nabnut_hoard
    {{0x62,0x01}, {0x46,0x06}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,au_flooded_room <- CCW,autumn

    //CCW,au_flooded_room
    {{0x63,0x01}, {0x45,0x06}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,au_flooded_room <- CCW,autumn
    //CCW,wi_flooded_room
    {{0x64,0x01}, {0x46,0x05}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,au_flooded_room <- CCW,autumn


    //CCW,sp_top_room
    {{0x65,0x01}, {0x43,0x08}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,spring <- CCW,sp_top_room
    //CCW,su_top_room
    {{0x66,0x01}, {0x44,0x08}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,summer <- CCW,su_top_room
    //CCW,au_top_room
    {{0x67,0x01}, {0x45,0x08}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,autumn <- CCW,au_top_room
    //CCW,wi_top_room
    {{0x68,0x01}, {0x46,0x08}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //CCW,autumn <- CCW,au_top_room

    //lair,mm_lobby
    {{0x69,0x02}, {0x02,0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair,mm_lobby <- MM,mm 
    {{0x69,0x12}, {0x01,0x13}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair,mm_loby <- SM,sm

    //lair, ttc_cc_puzzles
    {{0x6A,0x01}, {0x69,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}},//ttc_cc_puzzles <- mm_lobby
    {{0x6A,0x02}, {0x6B,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}},//ttc_cc_puzzles <- ccw_puzzle
    //0x08 purple cauldron

    //lair,ccw_puzzle
    {{0x6B,0x01},{0x6A,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}},//ccw_puzzle <- ttc_cc_puzzles
    {{0x6B,0x02},{0x6C,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}},//ccw_puzzle <- red_cauldron_room
    {{0x6B,0x03},{0x6D,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}},//ccw_puzzle <- ttc_lobby
    {{0x6B,0x04},{0x70,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}},//ccw_puzzle <- cc_lobby

    //lair,red_cauldron_room
    {{0x6C,0x01}, {0x6B,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}},//red_cauldron_room <- ccw_puzzle

    //lair,ttc_lobby
    {{0x6D,0x01},{0x6B,0x03}, {WF_HARD_NONE}, {WF_SOFT_NONE}},//ttc_lobby <- ccw_puzzle

    //lair, gv_lobby
    {{0x6E,0x01}, {0x71,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}},    //lair, GV_Lobby <- lair,260 note door
    {{0x6E,0x02}, {0x6F,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}},    //lair, GV_Lobby <- lair,fp_lobby

    //lair, fp_lobby
    {{0x6F,0x01}, {0x6E,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}},    //lair,fp_lobby <- lair, GV_Lobby

    //lair,cc_lobby
    {{0x70,0x01}, {0x6B, 0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair,cc_lobby <- CC,cc

    //260 note door
    {{0x71,0x02}, {0x6B,0x05}, {WF_HARD_NONE}, {WF_SOFT_NONE}},//260_nd <- ccw_puzzle
    {{0x71,0x03}, {0x72,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}},//260_nd <- bgs_lobby

    //lair,bgs_lobby
    {{0x72,0x01}, {0x71,0x03}, {WF_HARD_NONE}, {WF_SOFT_NONE}},//bgs_lobby <- 260_nd 

    //lair,gv_puzzle
    {{0x74,0x01}, {0x6F,0x05}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair,gv_puzzle <- lair,fp_lobby
    {{0x74,0x02}, {0x75,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair,gv_puzzle <- lair,mmm_lobby

    //lair,mmm_lobby
    {{0x75,0x01}, {0x74,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair,gv_puzzle <- lair,mmm_lobby

    //lair, 640_nd
    {{0x76,0x01}, {0x6F,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair, 640_nd <- lair,fp_lobby
    {{0x76,0x04}, {0x77,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair, 640_nd <- lair,rbb_lobby

    //lair,rbb_lobby
    {{0x77,0x01}, {0x76,0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair, 640_nd <- lair,rbb_lobby
    {{0x77,0x04}, {0x78,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair,rbb_lobby <- RBB,rbb_puzzle_mmm

    //rbb_mmm_puzzles
    //CHANCE TO SOFTLOCK (ALWAYS DESPAWN GRATE)
    {{0x78,0x02}, {0x77,0x04}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair,rbb_lobby <- RBB,rbb_puzzle_mmm

    //lair,ccw_lobby
    {{0x79,0x01}, {0x76,0x03}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair, ccw_lobby <- lair, 640_nd
    {{0x79,0x02}, {0x76,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair,ccw_lobby <- lair, 640_nd_upper
    
    //lair,crypt
    {{0x7A,0x01}, {0x75,0x03}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //MMM,crypt <- lair,mmm_lobby 

    //FP,wozza_cave
    {{0x7F,0x01}, {0x27,0x06}, {WF_HARD_NONE}, {WF_SOFT_NONE}},     //FP,fp <- FP,wozza 
    
    //lair,ff_lobby
    {{0x80,0x01}, {0x79,0x03}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair, 640_nd_upper <- lair,ff_lobby
    {{0x80,0x02}, {0x8E,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair, 640_nd_upper <- lair,ff_lobby

    //RBB,anchor_room
    {{0x8B,0x04}, {0x31,0x13}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //RBB,anchor_room <- RBB,rbb

    //SM,banjo_house
    {{0x8C,0x01},{0x01,0x12}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //SM,banjo_house <- SM,sm 

    //FF
    {{0x8E,0x01}, {0x80,0x02}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair, ff <- lair,ff_lobby
    {{0x8E,0x05}, {0x93,0x05}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //lair, ff <- lair,810_nd
    //yellow_cauldron
};

warp_t nodePool_unavailable_init[] ={
    //MM,mm
    {{0x02,0x03}, {0x0C,0x01}, {WF_HARD_NONE}, {0,0,BIT(WF_INACCESSIBLE)}}, //MM,mm <- MM,mm_tickers_upper

    //TTC, ttc
    {{0x07,0x06}, {0x05, 0x05}, {WF_HARD_NONE}, {BIT(WF_BEAK_BUST) | BIT(WF_EGGS), 0, 0}}, //TTC,ttc <- TTC,blubber_ship_top
    {{0x07,0x08}, {0x07, 0x0C}, {WF_HARD_NONE}, {BIT(WF_FLIGHT), 0, 0}},      //TTC,lighthouse_top <- TTC,lighthouse_bottom
    {{0x07,0x0C}, {0x07, 0x08}, {WF_HARD_NONE}, {BIT(WF_FLIGHT), 0, 0}},      //TTC,stairs_top <- TTC,stairs_bottom
    {{0x07,0x0E}, {0x07, 0x0F}, {WF_HARD_NONE}, {BIT(WF_FLIGHT), 0, 0}},      //TTC,stairs_bottom <- TTC,stairs_top
    {{0x07,0x0F}, {0x07, 0x0E}, {WF_HARD_NONE}, {BIT(WF_FLIGHT), 0, 0}},      //TTC,lighthouse_bottom <- TTC,lighthouse_top

    //CC, cc
    {{0x0B, 0x01}, {0x22,0x07}, {WF_HARD_NONE}, {BIT(WF_EGGS), 0, 0}}, //CC,cc <- CC,inside_left_tooth
    {{0x0B, 0x02}, {0x22,0x06}, {WF_HARD_NONE}, {BIT(WF_EGGS), 0, 0}}, //CC,cc <- CC,inside_right_tooth
    {{0x0B, NULL}, {0x21,0x01}, {WF_HARD_NONE}, {0,            0, BIT(WF_ONEWAY_OUT)}}, ////CC,witch_switch_room <- CC,cc

    //MM,mm_tickers
    {{0x0C,0x01}, {0x02,0x03}, 0, {WF_HARD_NONE}, {BIT(WF_BEAK_BUST), 0, 0}},  //MM,mm_tickers_upper <- MM,mm 

    //BGS, bgs
    {{0x0D,0x03}, {0x11,0x01}, {WF_HARD_NONE}, {BIT(WF_BEAK_BUST),   0, 0}},     //BGS,bgs <- BGS,tanktup
    {{0x0D,0x04}, {0x10,0x04}, {WF_HARD_NONE}, {0, BIT(WF_TRANS_CROC), 0}},     //BGS,bgs <- BGS,vile_right  
    {{0x0D,0x05}, {0x10,0x05}, {WF_HARD_NONE}, {0, BIT(WF_TRANS_CROC), 0}},     //BGS,bgs <- BGS,vile_left  
    {{0x0D,0x06}, {0x47,0x01}, {WF_HARD_NONE}, {BIT(WF_WADING_BOOTS), 0, 0}},     //BGS,bgs <- BGS,bgs_mumbos  

    //BGS, vile
    {{0x10,0x04}, {0x0D,0x04}, {WF_HARD_NONE}, {0, BIT(WF_TRANS_CROC), 0}},     //BGS,vile_right <- BGS,bgs   
    {{0x10,0x05}, {0x0D,0x05}, {WF_HARD_NONE}, {0, BIT(WF_TRANS_CROC), 0}},     //BGS,vile_left <- BGS,bgs  

    //GV,gv
    {{0x12,0x02}, {0x1A,0x01}, {WF_HARD_NONE}, {BIT(WF_EGGS),   0, 0}},      //GV,gv <- GV,jinxy
    {{0x12,0x03}, {0x13,0x01}, {WF_HARD_NONE}, {BIT(WF_BEAK_BUST),   0, 0}}, //GV,gv <- GV,matching_game
    {{0x12,0x04}, {0x14,0x01}, {WF_HARD_NONE}, {BIT(WF_EGGS),   0, 0}},      //GV,gv <- GV,sandybutt_front
    {{0x12,0x05}, {0x15,0x06}, {WF_HARD_NONE}, {0, 0, BIT(WF_GV_WATER)}},  //GV,gv <- GV,water_pyramid
    {{0x12,0x06}, {0x16,0x07}, {WF_HARD_NONE}, {BIT(WF_BEAK_BOMB) | BIT(WF_FLIGHT),   0, 0}},  //GV,gv <- GV,rubee
    {{0x12,0x07}, {0x14,NULL}, {WF_HARD_NONE}, {0, 0, BIT(WF_ONEWAY_IN)}},    //GV,gv <- GV,sandybutt_back 
    {{0x12,NULL}, {0x15,0x02}, {WF_HARD_NONE}, {BIT(WF_TURBO_TALON) | BIT(WF_FLIGHT),   0, BIT(WF_ONEWAY_OUT)}},    //GV,gv <- GV,water_pyramid_top 

    //GV,sandybutt
    {{0x14,NULL}, {0x12,0x07}, {WF_HARD_NONE}, {0, 0, BIT(WF_ONEWAY_OUT)}}, //GV,sandybutt_back <- GV,gv  

    //GV,water_pyramid
    //0x01 cutscene return
    {{0x15,0x02}, {0x12,NULL}, {WF_HARD_NONE}, {0, 0, BIT(WF_ONEWAY_IN)}}, //GV,water_pyramid_top <- GV,gv

    //MMM,mmm
    {{0x1B,NULL}, {0x26,0x02}, {WF_HARD_NONE}, {0, 0, BIT(WF_ONEWAY_OUT)}}, //MMM,dining_room_chimney 
    {{0x1B,0x05}, {0x1C,0x01}, {0,0,BIT(WF_BEAK_BUST) | BIT(WF_TURBO_TALON)}, {WF_SOFT_NONE}}, //MMM,mmm <- MMM,church
    {{0x1B,0x08}, {0x2F,0x02}, {WF_HARD_NONE}, {0, BIT(WF_TRANS_PUMPKIN), 0}}, //MMM,mmm <- MMM,rain_barrel_bottom
    {{0x1B,NULL}, {0x2F,0x01}, {WF_HARD_NONE}, {0, BIT(WF_TRANS_PUMPKIN), BIT(WF_ONEWAY_OUT)}}, //MMM,rain_barrel_top
    {{0x1B,0x11}, {0x1B,0x10}, {WF_HARD_NONE}, {0, 0, BIT(WF_INACCESSIBLE)}}, //MMM,mmm_clock_top <- MMM,mmm_clock_bottom
    {{0x1B,0x13}, {0x25,0x04}, {WF_HARD_NONE}, {0, BIT(WF_TRANS_PUMPKIN), 0}}, //MMM,mmm <- MMM,well_bottom

    //CC, inside
    {{0x22,0x01}, {0x21,NULL}, {WF_HARD_NONE}, {0, 0, BIT(WF_ONEWAY_IN)}}, //CC,inside <-CC,ws_jiggy
    {{0x22,0x02}, {0x21,NULL}, {WF_HARD_NONE}, {0, 0, BIT(WF_ONEWAY_IN)}},  //CC,inside <-CC,ws_ws
    {{0x22,0x03}, {0x23,0x01}, {WF_HARD_NONE}, {BIT(WF_FLIGHT), 0, 0}},  //CC,inside <-CC,wonderwing
    {{0x22,0x06}, {0x0B,0x02}, {WF_HARD_NONE}, {0, 0, BIT(WF_INACCESSIBLE)}}, //CC,inside_right_tooth    <- CC,cc
    {{0x22,0x07}, {0x0B,0x01}, {WF_HARD_NONE}, {0, 0, BIT(WF_INACCESSIBLE)}}, //CC,inside_left_tooth     <- CC,cc

    //MMM,well_top
    {{0x25,0x04}, {0x1B,0x13}, {WF_HARD_NONE}, {0, BIT(WF_TRANS_PUMPKIN), 0}}, //MMM,well_bottom <- MMM,mmm

    //MMM,dining_room
    {{0x26,0x02}, {0x1B,NULL}, {WF_HARD_NONE}, {0, 0, BIT(WF_ONEWAY_IN)}},     //MMM,dining_room_chimney <- MMM,mmm 

    //FP,fp
    {{0x27,0x01}, {0x6F,0x06}, {WF_HARD_NONE}, {BIT(WF_TALON_TROT), 0, 0}}, //FP,fp <- FP,lobby 
    {{0x27,0x06}, {0x7F,0x01}, {WF_HARD_NONE}, {0, BIT(WF_TRANS_WALRUS), 0}},     //FP,fp <- FP,wozza
    {{0x27,0x08}, {0x41,0x01}, {WF_HARD_NONE}, {BIT(WF_TALON_TROT), 0, 0}}, //FP,fp <- FP,igloo 

    //MMM,bathroom
    {{0x2C,0x04}, {0x8D,0x04}, {WF_HARD_NONE}, {0, BIT(WF_TRANS_PUMPKIN), 0}}, //MMM,bathroom <- MMM,logo

    //MMM,rain_barrel
    {{0x2F,0x01}, {0x1B,NULL}, {WF_HARD_NONE}, {0, 0, BIT(WF_ONEWAY_IN)}}, //MMM,mmm <- MMM,rain_barrel_top

    //RBB, rbb
    {{0x31,NULL}, {0x36,0x02}, {WF_HARD_NONE}, {0, 0, BIT(WF_ONEWAY_OUT)}}, //RBB,rbb <- RBB,warehouse_skylight

    //RBB,warehouse
    {{0x36,0x02}, {0x31,NULL}, {WF_HARD_NONE}, {0, 0, BIT(WF_ONEWAY_IN)}}, //RBB,warehouse_skylight <- RBB,rbb

    //CCW,hub
    {{0x40,0x01}, {0x46,0x01}, {BIT(WF_OPEN_WINTER), 0, 0},{WF_SOFT_NONE}}, //CCW,hub <- CCW,winter
    {{0x40,0x02}, {0x43,0x01}, {BIT(WF_OPEN_SPRING), 0, 0},{WF_SOFT_NONE}}, //CCW,hub <- CCW,spring
    {{0x40,0x03}, {0x44,0x01}, {BIT(WF_OPEN_SUMMER), 0, 0},{WF_SOFT_NONE}}, //CCW,hub <- CCW,summer
    {{0x40,0x04}, {0x45,0x01}, {BIT(WF_OPEN_AUTUMN), 0, 0},{WF_SOFT_NONE}}, //CCW,hub <- CCW,autumn

    //CCW_Spring
    {{0x43,0x04}, {0x5E,0x02}, {WF_HARD_NONE}, {BIT(WF_TALON_TROT) | BIT(WF_SHOCK_SPRING), 0, 0}}, //CCW,spring <- CCW,sp_nabnuts_window
    {{0x43,0x06}, {0x5A,0x01}, {WF_HARD_NONE}, {0, BIT(WF_TRANS_BEE), 0}}, //CCW,spring <- CCW,sp_zubbas
    {{0x43,0x07}, {0x5E,0x01}, {WF_HARD_NONE}, {BIT(WF_TALON_TROT) | BIT(WF_SHOCK_SPRING), 0, 0}}, //CCW,spring <- CCW,sp_nabnuts_door
    {{0x43,0x08}, {0x65,0x01}, {WF_HARD_NONE}, {BIT(WF_TALON_TROT) | BIT(WF_SHOCK_SPRING), 0, 0}}, //CCW,spring <- CCW,sp_top_room

    //CCW_Summer
    {{0x44,0x04}, {0x5F,0x02}, {WF_HARD_NONE}, {BIT(WF_TALON_TROT) | BIT(WF_SHOCK_SPRING), 0, 0}}, //CCW,spring <- CCW,su_nabnuts_window
    {{0x44,0x06}, {0x5B,0x02}, {0,0,BIT(WF_BEAK_BUST)}, {BIT(WF_TALON_TROT) | BIT(WF_SHOCK_SPRING), 0, 0}}, //CCW,spring <- CCW,su_zubbas
    {{0x44,0x07}, {0x5F,0x01}, {WF_HARD_NONE}, {BIT(WF_TALON_TROT) | BIT(WF_SHOCK_SPRING), 0, 0}}, //CCW,spring <- CCW,su_nabnuts_door
    {{0x44,0x08}, {0x66,0x01}, {WF_HARD_NONE}, {BIT(WF_TALON_TROT) | BIT(WF_SHOCK_SPRING), 0, 0}}, //CCW,spring <- CCW,su_top_room

    //CCW_AUTUMN
    {{0x45,0x04}, {0x60,0x02}, {WF_HARD_NONE}, {BIT(WF_TALON_TROT) | BIT(WF_SHOCK_SPRING), 0, 0}}, //CCW,spring <- CCW,au_nabnuts_window
    {{0x45,0x05}, {0x5C,0x01}, {0,0,BIT(WF_BEAK_BUST)}, {BIT(WF_TALON_TROT) | BIT(WF_SHOCK_SPRING), 0, 0}}, //CCW,spring <- CCW,au_zubbas
    {{0x45,0x06}, {0x63,0x02}, {WF_HARD_NONE}, {BIT(WF_TALON_TROT) | BIT(WF_SHOCK_SPRING), 0, 0}}, //CCW,spring <- CCW,au_flooded_room
    {{0x45,0x07}, {0x60,0x01}, {WF_HARD_NONE}, {BIT(WF_TALON_TROT) | BIT(WF_SHOCK_SPRING), 0, 0}}, //CCW,spring <- CCW,au_nabnuts_door
    {{0x45,0x08}, {0x67,0x01}, {WF_HARD_NONE}, {BIT(WF_TALON_TROT) | BIT(WF_SHOCK_SPRING), 0, 0}}, //CCW,spring <- CCW,au_top_room

    //CCW,winter
    {{0x46,0x04}, {0x61,0x02}, {WF_HARD_NONE}, {BIT(WF_TALON_TROT) | BIT(WF_FLIGHT), 0, 0}}, //CCW,spring <- CCW,au_nabnuts_window
    {{0x46,0x05}, {0x64,0x01}, {0,0,BIT(WF_BEAK_BUST)}, {BIT(WF_TALON_TROT) | BIT(WF_SHOCK_SPRING), 0, 0}}, //CCW,spring <- CCW,au_zubbas
    {{0x46,0x06}, {0x62,0x01}, {WF_HARD_NONE}, {BIT(WF_FLIGHT), 0, 0}}, //CCW,spring <- CCW,au_flooded_room

    //CCW,zubbas
    {{0x5A,0x01}, {0x43,0x06}, {WF_HARD_NONE}, {0, BIT(WF_TRANS_BEE), 0}}, //CCW,sp_zubbas <- CCW,spring

    //lair,mm_lobby
    {{0x69,0x01}, {0x6A,0x01}, {0, BIT(WF_50_NOTES), 0},{WF_SOFT_NONE}}, //lair,mm_lobby <- lair,ttc_cc_puzzles 

    //lair,ccw_puzzle
    {{0x6B,0x05},{0x71,0x02}, {0, BIT(WF_180_NOTES), BIT(WF_TALON_TROT)},{WF_SOFT_NONE}},//ccw_puzzle <- 260_nd

    //lair,ttc_lobby
    {{0x6D,0x04},{0x07,0x04}, {BIT(WF_OPEN_TTC), 0, 0},{WF_SOFT_NONE}},//ttc_lobby <- TTC,ttc

    //lair, gv_lobby
    {{0x6E,0x03}, {0x12,0x08}, {BIT(WF_OPEN_GV), 0, 0},{WF_SOFT_NONE}}, //lair, GV_Lobby <- GV,gv
    {{0x6E,NULL}, {0x71,0x04}, {WF_HARD_NONE}, {BIT(WF_BEAK_BUST) | BIT(WF_SHOCK_SPRING), 0, BIT(WF_ONEWAY_OUT)}},//gv_lobby_vase - 260_nd 

    //lair, fp_lobby
    {{0x6F,0x02}, {0x76,0x01}, {0,BIT(WF_450_NOTES),0},{WF_SOFT_NONE}},    //lair,fp_lobby <- lair, 640_nd
    {{0x6F,0x05}, {0x73,0x02}, {0,BIT(WF_350_NOTES),0},{WF_SOFT_NONE}},    //lair,fp_lobby <- lair, gv_puzzle
    {{0x6F,0x06}, {0x27,0x01}, {BIT(WF_OPEN_FP), 0, 0},{WF_SOFT_NONE}},    //lair,fp_lobby <- FP,fp
    
    //lair,cc_lobby
    {{0x70,0x02}, {0x0B, 0x05}, {BIT(WF_OPEN_CC), 0, BIT(WF_BEAK_BUST)},{WF_SOFT_NONE}}, //lair,cc_lobby <- CC,cc

    //260 note door
    {{0x71,0x01}, {0x6E,0x01}, {0, BIT(WF_260_NOTES), BIT(WF_SHOCK_SPRING)},{WF_SOFT_NONE}},//260_nd <- gv_lobby_260door
    {{0x71,0x04}, {0x6E,NULL}, {WF_HARD_NONE}, {0, 0, BIT(WF_ONEWAY_IN)}},//260_nd <- gv_lobby_vase

    //lair,bgs_lobby
    {{0x72,0x02}, {0x0D,0x02}, {BIT(WF_OPEN_BGS), 0, 0},{WF_SOFT_NONE}}, //lair,bgs_lobby <- BGS,bgs

    //lair,mmm_lobby
    {{0x75,0x02}, {0x1B,0x14}, {BIT(WF_OPEN_MMM), 0, 0},{WF_SOFT_NONE}}, //lair,mmm_lobby <- MMM,mmm
    {{0x75,0x03}, {0x7A,0x01}, {WF_HARD_NONE}, {0, BIT(WF_TRANS_PUMPKIN), 0}}, //lair,mmm_lobby <- MMM,crypt

    //lair, 640_nd
    {{0x76,0x02}, {0x79,0x02}, {WF_HARD_NONE}, {0, 0, BIT(WF_INACCESSIBLE)}}, //lair, 640_nd <- lair,rbb_lobby
    {{0x76,0x03}, {0x79,0x01}, {0, BIT(WF_640_NOTES), 0}, {0, 0, BIT(WF_WATER_LVL2)}}, //lair, 640_nd <- lair,rbb_lobby 

    //lair,rbb_lobby
    {{0x77,0x02}, {0x31,0x10}, {BIT(WF_OPEN_RBB), 0, 0}, {0, 0, BIT(WF_WATER_LVL2)}}, //lair,rbb_lobby <- RBB,rbb
    {{0x77,0x03}, {0x78,0x01}, {WF_HARD_NONE}, {0, 0, BIT(WF_WATER_LVL3)}}, //lair,rbb_lobby <- RBB,rbb_puzzle_upper

    //rbb_mmm_puzzles
    //CHANCE TO SOFTLOCK (ALWAYS DESPAWN GRATE)
    {{0x78,0x01}, {0x77,0x03}, {WF_HARD_NONE}, {0, 0, BIT(WF_WATER_LVL2)}}, //lair,rbb_lobby <- RBB,rbb_puzzle_rbb

    //lair,ccw_lobby
    {{0x79,0x03}, {0x80,0x01}, {0, BIT(WF_765_NOTES), 0},{WF_SOFT_NONE}}, //lair,ccw_lobby <- lair,ff_lobby
    {{0x79,0x06}, {0x40,0x07}, {BIT(WF_OPEN_DOG), 0, BIT(WF_TALON_TROT)},{WF_SOFT_NONE}}, //lair,ccw_lobby <- CCW,hub

    //MMM,logo
    {{0x8D,0x04}, {0x2C,0x04}, {WF_HARD_NONE}, {0, BIT(WF_TRANS_PUMPKIN), 0}}, //MMM,logo <- MMM,bathroom

    //lair 810_nd
    //TODO this room sucks
    {{0x93,0x01}, {0x90,0x01}, {BIT(WF_OPEN_DOG), 0, 0},{WF_SOFT_NONE}}, //lair,810_nd <- FIGHT
    {{0x93,0x05}, {0x8E,0x05}, {0, BIT(WF_810_NOTES), 0},{WF_SOFT_NONE}}, //lair,810_nd <- lair, ff
    //0x08 yellow cauldron
};

warp_t tree_available_init[] ={
    // SM,sm
    {{0x01,0x12}, {0x8C,0x01}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //SM,sm <- SM,banjo_house
    {{0x01,0x13}, {0x69,0x12}, {WF_HARD_NONE}, {WF_SOFT_NONE}}, //SM,sm <- lair,mm_loby
};

warp_t tree_unavailable_init[] ={
    //only way to get these moves moved over

    //CC, witch_switch_room
    {{0x21,0x01}, {0x0B,NULL}, {WF_HARD_NONE}, {0, 0, BIT(WF_ONEWAY_IN)}}, //CC,witch_switch_room <- CC,cc
    {{0x21,NULL}, {0x22,0x01}, {WF_HARD_NONE}, {0, 0, BIT(WF_ONEWAY_OUT)}}, // CC,inside_jiggy
    {{0x21,NULL}, {0x22,0x02}, {WF_HARD_NONE}, {0, 0, BIT(WF_ONEWAY_OUT)}}, // CC,inside_ws

    //FIGHT
    //TODO handle seperate
    {{0x90,0x01}, {0x93,0x01}, {WF_HARD_NONE}, {0, 0, BIT(WF_FIGHT)}}, //lair,810_nd <- FIGHT
};

void warp_mapping_init(void){
    nodePool_a.len = NODEPOOL_A_INIT_SIZE;
    for(int i = 0; i < nodePool_a.len; i++){
        nodePool_a.pool[i] = &(nodePool_available_init[i]);
    }

    nodePool_u.len = NODEPOOL_U_INIT_SIZE;
    for(int i = 0; i < nodePool_u.len; i++){
        nodePool_u.pool[i] = &(nodePool_unavailable_init[i]);
    }


    tree_a.len = TREE_A_INIT_SIZE;
    for(int i = 0; i < tree_a.len; i++){
        tree_a.pool[i] = &(tree_available_init[i]);
    }

    tree_u.len = TREE_U_INIT_SIZE;
    for(int i = 0; i < tree_u.len; i++){
        tree_u.pool[i] = &(tree_unavailable_init[i]);
    }

    exit_t spiral_mountain_seed = {SM_SM, 0};
    banjo_tree = tree_init(&spiral_mountain_seed, &tree_a, &tree_u);

    tree_includes_map_flag = 0;
    warp_hard_flags_t tmp_hard = {WF_HARD_NONE};
    warp_soft_flags_t tmp_soft = {WF_SOFT_NONE};
    memcpy(&achieved_hard_flags, &tmp_hard, sizeof(warp_hard_flags_t));
    memcpy(&achieved_soft_flags, &tmp_soft, sizeof(warp_soft_flags_t));
    tree_jiggy_cnt = 0;
    tree_token_cnt = 0;
    tree_note_cnt  = 0;
}

int warp_nodePool_randSelect(void){
    u32 np_i;
    u32 tree_i;
    warp_t* selected_nodePool;
    warp_t* selected_exit;
    if(tree_a.len == 1){
        u32 start_indx = warp_rand()% nodePool_a.len;
        u32 currCandidate = start_indx;
        do{
            if(!(nodePool_a.pool[currCandidate]->me.map == TTC_SHIP)
            && !(nodePool_a.pool[currCandidate]->me.map == RBB_ENGINE)){
                for(int i = 0; i< nodePool_a.len; i++){
                    //find nodePool with more than 1 available exit
                    if((i != currCandidate) 
                    && (nodePool_a.pool[i]->me.map == nodePool_a.pool[currCandidate]->me.map)){
                        return currCandidate;
                    }
                }
            }
            currCandidate++;
            currCandidate = currCandidate % nodePool_a.len;

        }
        while (currCandidate != start_indx);
        return -1;
    }
    else{
        return warp_rand()% nodePool_a.len;
    }
}

void warp_attachWarps(int pool_i, int tree_i){
    //make connection
    exitLUT_set(&(tree_a.pool[tree_i]->ret), &(nodePool_a.pool[pool_i]->me));
    exitLUT_set(&(nodePool_a.pool[pool_i]->ret), &(tree_a.pool[tree_i]->me));
    u8 map_index = _warp_get_map_index(&(nodePool_a.pool[pool_i]->me));
    //delete nodePool warp
    if(nodePool_a.len > 0){
        nodePool_a.pool[pool_i] = nodePool_a.pool[nodePool_a.len-1];
        nodePool_a.len--;
    }

    //delete tree warp
    tree_a.pool[tree_i] = tree_a.pool[--tree_a.len];
    //move available exits
    if(nodePool_a.len != 0){
        for(int i = nodePool_a.len-1; i >= 0; i--){
            if(_warp_get_map_index(&(nodePool_a.pool[i]->me)) == map_index){
                //increase tree_size
                tree_a.len++;
                //copy exit to tree
                tree_a.pool[tree_a.len-1] = nodePool_a.pool[i];
                //remove from nodePool
                nodePool_a.pool[i] = nodePool_a.pool[--nodePool_a.len];
                
            }
        }
    }
    //move unavailable exits
    if(nodePool_u.len == 0) return;
    for(int i = nodePool_u.len-1; i >= 0; i--){
        if(_warp_get_map_index(&(nodePool_u.pool[i]->me)) == map_index){
            //increase tree_size
            tree_u.len++;
            //copy exit to tree
            tree_u.pool[tree_u.len-1] = nodePool_u.pool[i];
            //remove from nodePool
            nodePool_u.pool[i] = nodePool_u.pool[--nodePool_u.len];
        }
    }
}

static bool _warp_hard_flags_met(warp_hard_flags_t* req_flags, warp_hard_flags_t* achieved_flags ){
    if(req_flags->level_open & ~(achieved_flags->level_open))
        return false;
    else if(req_flags->note_door & ~(achieved_flags->note_door))
        return false;
    else if(req_flags->moves & ~(achieved_flags->moves))
        return false;
    else 
        return true;
}

static bool _warp_soft_flags_met(warp_soft_flags_t* req_flags, warp_soft_flags_t* achieved_flags ){
    if(((req_flags->moves & ~(achieved_flags->moves)) == req_flags->moves)
    && (req_flags->moves != 0))
        return false;
    else if(((req_flags->transform & ~(achieved_flags->transform)) == req_flags->transform)
    && (req_flags->transform != 0))
        return false;
    else if(((req_flags->misc & ~(achieved_flags->misc)) == req_flags->misc)
    && (req_flags->misc != 0))
        return false;
    else
        return true;
}

void warp_update_availability(void){
    //move unavailable nodePool exits
    if(nodePool_u.len != 0 ){
        for(int i = 0; i < nodePool_u.len; i++){
            warp_t* curr_exit = nodePool_u.pool[nodePool_u.len-1-i];
            if(_warp_hard_flags_met(&curr_exit->hard_flags, &banjo_tree->hard_f)
            && _warp_soft_flags_met(&curr_exit->soft_flags, &banjo_tree->soft_f)){
                //increase nodePool_a.pool size
                nodePool_a.len++;
                //copy exit to nodePool_a.pool
                nodePool_a.pool[nodePool_a.len-1] = curr_exit;
                //remove from nodePool_u.pool
                nodePool_u.pool[nodePool_u.len-1-i] = nodePool_u.pool[nodePool_u.len-1];
                nodePool_u.len--;
            }
        }
    }

    //move unavailable tree exits
    if(tree_u.len != 0 ){
        for(int i = 0; i < tree_u.len; i++){
            warp_t* curr_exit = tree_u.pool[tree_u.len-1-i];
            if(_warp_hard_flags_met(&curr_exit->hard_flags, &banjo_tree->hard_f)
            && _warp_soft_flags_met(&curr_exit->soft_flags, &banjo_tree->soft_f)){
                //increase tree_u.pool size
                tree_a.len++;
                //copy exit to tree_u.pool
                tree_a.pool[tree_a.len-1] = curr_exit;
                //remove from tree_u.pool
                tree_u.pool[tree_u.len-1-i] = tree_u.pool[tree_u.len-1];
                tree_u.len--;
            }
        }
    }
}

void wm_generate_mapping(u32 seed){
    exitLUT_clear();
    warp_mapping_init();
    warp_srand(seed);
    tree_note_cnt = 6;

    D_PRINTF("POOL u=%3d a=%3d : TREE u=%3d a=%3d\n",
            nodePool_u.len, nodePool_a.len,
            tree_u.len, tree_a.len);

    int i = 0;
    while (nodePool_a.len != 0){// not all nodes in tree
        //select exit from nodePool
        int s_exit = warp_nodePool_randSelect();
        if(s_exit == -1){
             while(nodePool_a.len){
                warp_t** fail = &nodePool_a.pool[--nodePool_a.len];
                D_PRINTF("FAIL_A: %3d: attaching {%02x,%02x}\n", nodePool_a.len, (*fail)->me.map, (*fail)->me.exit);
             }
             while(nodePool_u.len){
                warp_t** fail = &nodePool_u.pool[--nodePool_u.len];
                D_PRINTF("FAIL_U: %3d: attaching {%02x,%02x}\n", nodePool_u.len, (*fail)->me.map, (*fail)->me.exit);
             }
             break;
        }
        warp_t* selected_exit =  nodePool_a.pool[s_exit];
         //error: unable to select final nodes without softlocking breaking for now
        u8 added_map = (*selected_exit).me.map;
        //select exit from tree
        int s_ent = warp_rand()%tree_a.len;
        warp_t* selected_entrance = tree_a.pool[s_ent];

        //attach
        D_PRINTF("%3d: attaching [%02x]{%02x,%02x} <-> [%02x]{%02x,%02x} :\n", ++i,
            s_exit,(*selected_exit).me.map, (*selected_exit).me.exit,
            s_ent, (*selected_entrance).me.map, (*selected_entrance).me.exit);

        //TODO: account for transformations

        warp_attachWarps(s_exit, s_ent);
        tree_attach_exit(banjo_tree, selected_entrance, selected_exit);
        warp_update_availability();

         D_PRINTF("    POOL u=%3d a=%3d : TREE u=%3d a=%3d : %d\n",
            nodePool_u.len, nodePool_a.len,
            tree_u.len, tree_a.len,
            i*2 + nodePool_u.len + nodePool_a.len + tree_u.len + tree_a.len);
    };
     D_PRINTF("Notes: %d, Jiggies %d, Tokens: %d\n", banjo_tree->note_count, bitfield_get_count(banjo_tree->jiggies), banjo_tree->token_count);
    //make all unavailable nodes
    while(nodePool_u.len){
        warp_t** fail = &nodePool_u.pool[--nodePool_u.len];
        D_PRINTF("FAIL: %3d: attaching {%02x,%02x}\n", nodePool_u.len, (*fail)->me.map, (*fail)->me.exit);
    }
   
    //ATTACH FIGHT!!!
    D_PRINTF("ATTACHING FIGHT\n");
    int fight_indx = -1;
    int j = 0;
    for(int j =0; j < tree_u.len; j++){
        
        if((tree_u.pool[j]->soft_flags.misc) & BIT(WF_FIGHT)){
            fight_indx = j;
        }
    }
    int rand_start = warp_rand()%tree_a.len;
    int curr_indx = rand_start;
    int selected_indx = rand_start;
    u8 selected_depth = 0;
    do{
        u8 curr_depth = tree_get_exit_depth(banjo_tree,&(tree_a.pool[curr_indx]->me));
        if(curr_depth > selected_depth){
            selected_depth = curr_depth;
            selected_indx = curr_indx;
        }
        curr_indx++;
        curr_indx = curr_indx%tree_a.len;
    }while(curr_indx != rand_start);

    D_PRINTF("%3d: attaching [%02x]{%02x,%02x} <-> [%02x]{%02x,%02x} :\n",++i,
            fight_indx, tree_u.pool[fight_indx]->me.map, tree_u.pool[fight_indx]->me.exit,
            selected_indx, tree_a.pool[selected_indx]->me.map,  tree_a.pool[selected_indx]->me.exit);
    //make connection
    exitLUT_set(&(tree_a.pool[selected_indx]->ret), &(tree_u.pool[fight_indx]->me));
    exitLUT_set(&(tree_u.pool[fight_indx]->ret), &(tree_a.pool[selected_indx]->me));
    tree_attach_exit(banjo_tree, &(tree_u.pool[fight_indx]->me), &(tree_u.pool[fight_indx]->me));
    //delete fight warp
    tree_u.pool[fight_indx] = tree_u.pool[tree_u.len--];
    //delete tree warp
    tree_a.pool[selected_indx] = tree_a.pool[--tree_a.len];
    
    D_PRINTF("    POOL u=%3d a=%3d : TREE u=%3d a=%3d : %d\n",
            nodePool_u.len, nodePool_a.len,
            tree_u.len, tree_a.len,
            i*2 + nodePool_u.len + nodePool_a.len + tree_u.len + tree_a.len);

    //ATTACH CC WS_ROOM IN
    D_PRINTF("ATTACHING CC WS_ROOM\n");
    int s_ent = warp_rand()%tree_a.len;
    int s_exit = 0;
    D_PRINTF("%3d: attaching [%02x]{%02x,%02x} <-> [%02x]{%02x,%02x} :\n",++i,
            s_exit, tree_u.pool[s_exit]->me.map, tree_u.pool[s_exit]->me.exit,
            s_ent, tree_a.pool[s_ent]->me.map,  tree_a.pool[s_ent]->me.exit);
    //make connection
    exitLUT_set(&(tree_a.pool[s_ent]->ret), &(tree_u.pool[s_exit]->me));
    tree_attach_exit(banjo_tree, &(tree_u.pool[s_ent]->me), &(tree_u.pool[s_exit]->me));
    //delete exits
    tree_u.pool[s_exit] = tree_u.pool[--tree_u.len];
    tree_a.pool[s_ent] = tree_a.pool[--tree_a.len];
    
    D_PRINTF("    POOL u=%3d a=%3d : TREE u=%3d a=%3d : %d\n",
            nodePool_u.len, nodePool_a.len,
            tree_u.len, tree_a.len,
            i*2 + nodePool_u.len + nodePool_a.len + tree_u.len + tree_a.len);

    //ATTACH INACCESSIBLE
    D_PRINTF("ATTACHING INACCESSIBLE EXITS\n");
    for(int j = tree_u.len-1; j >= 0 ; j--){
        if(tree_u.pool[j]->soft_flags.misc & BIT(WF_INACCESSIBLE)){
            nodePool_a.pool[nodePool_a.len++] = tree_u.pool[j];
            tree_u.pool[j] = tree_u.pool[--tree_u.len];
        }
    }
    D_PRINTF("    POOL u=%3d a=%3d : TREE u=%3d a=%3d\n",
            nodePool_u.len, nodePool_a.len,
            tree_u.len, tree_a.len);
    while(nodePool_a.len != 0 && tree_a.len !=0){
        int s_exit = warp_rand()%nodePool_a.len;
        u8 oneway_depth = tree_get_exit_depth(banjo_tree,&(nodePool_a.pool[s_exit]->me));

        int rand_start = warp_rand()%tree_a.len;
        int curr_indx = rand_start;
        int s_ent = rand_start;
        u8 selected_depth = 0;
        do{
            //TODO change requirement to being a parent node
            u8 curr_depth = tree_get_exit_depth(banjo_tree,&(tree_a.pool[curr_indx]->me));
            if(curr_depth < oneway_depth){
                s_ent = curr_indx;
                break;
            }
            if(curr_depth < selected_depth){
                selected_depth = curr_depth;
                s_ent = curr_indx;
            }
            curr_indx++;
            curr_indx = curr_indx%tree_a.len;
        }while(curr_indx != rand_start);

        D_PRINTF("%3d: attaching [%02x]{%02x,%02x} <-> [%02x]{%02x,%02x} :\n",++i,
            s_exit, nodePool_a.pool[s_exit]->me.map, nodePool_a.pool[s_exit]->me.exit,
            s_ent, tree_a.pool[s_ent]->me.map,  tree_a.pool[s_ent]->me.exit);
        //make connection
        exitLUT_set(&(nodePool_a.pool[s_exit]->ret), &(tree_a.pool[s_ent]->me));
        exitLUT_set(&(tree_a.pool[s_ent]->ret), &(nodePool_a.pool[s_exit]->me));
        //delete exits
        nodePool_a.pool[s_exit] = nodePool_a.pool[--nodePool_a.len];
        tree_a.pool[s_ent] = tree_a.pool[--tree_a.len];
    }
     D_PRINTF("    POOL u=%3d a=%3d : TREE u=%3d a=%3d\n",
            nodePool_u.len, nodePool_a.len,
            tree_u.len, tree_a.len);


    //ATTACH ONE WAY INs - must attach to node further in tree
     D_PRINTF("ATTACHING ONEWAY IN EXITS\n");
    for(int j = tree_u.len-1; j >= 0 ; j--){
        if(tree_u.pool[j]->soft_flags.misc & BIT(WF_ONEWAY_IN)){
            nodePool_a.pool[nodePool_a.len++] = tree_u.pool[j];
            tree_u.pool[j] = tree_u.pool[--tree_u.len];
        }
    }
    D_PRINTF("    POOL u=%3d a=%3d : TREE u=%3d a=%3d\n",
            nodePool_u.len, nodePool_a.len,
            tree_u.len, tree_a.len);
    while(nodePool_a.len != 0 && tree_a.len !=0){
        int s_exit = warp_rand()%nodePool_a.len;
        u8 oneway_depth = tree_get_exit_depth(banjo_tree,&(nodePool_a.pool[s_exit]->me));

        int rand_start = warp_rand()%tree_a.len;
        int curr_indx = rand_start;
        int s_ent = rand_start;
        u8 selected_depth = 0;
        do{
            u8 curr_depth = tree_get_exit_depth(banjo_tree,&(tree_a.pool[curr_indx]->me));
            if(curr_depth > oneway_depth){
                s_ent = curr_indx;
                break;
            }
            if(curr_depth > selected_depth){
                selected_depth = curr_depth;
                s_ent = curr_indx;
            }
            curr_indx++;
            curr_indx = curr_indx%tree_a.len;
        }while(curr_indx != rand_start);

        D_PRINTF("%3d: attaching [%02x]{%02x,%02x} <-> [%02x]{%02x,%02x} :\n",++i,
            s_exit, nodePool_a.pool[s_exit]->me.map, nodePool_a.pool[s_exit]->me.exit,
            s_ent, tree_a.pool[s_ent]->me.map,  tree_a.pool[s_ent]->me.exit);
        //make connection
        exitLUT_set(&(tree_a.pool[s_ent]->ret), &(nodePool_a.pool[s_exit]->me));
        //delete exits
        nodePool_a.pool[s_exit] = nodePool_a.pool[--nodePool_a.len];
        tree_a.pool[s_ent] = tree_a.pool[--tree_a.len];
    }
    D_PRINTF("    POOL u=%3d a=%3d : TREE u=%3d a=%3d\n",
            nodePool_u.len, nodePool_a.len,
            tree_u.len, tree_a.len);

    //ATTACH ONE WAY OUTs
    D_PRINTF("ATTACHING ONEWAY OUT EXITS\n");
    for(int j = tree_u.len-1; j >= 0 ; j--){
        if(tree_u.pool[j]->soft_flags.misc & BIT(WF_ONEWAY_OUT)){
            nodePool_a.pool[nodePool_a.len++] = tree_u.pool[j];
            tree_u.pool[j] = tree_u.pool[--tree_u.len];
        }
    }
    D_PRINTF("    POOL u=%3d a=%3d : TREE u=%3d a=%3d\n",
            nodePool_u.len, nodePool_a.len,
            tree_u.len, tree_a.len);
    while(nodePool_a.len != 0 && tree_a.len !=0){
        int s_exit = warp_rand()%nodePool_a.len;
        u8 oneway_depth = tree_get_exit_depth(banjo_tree,&(nodePool_a.pool[s_exit]->me));

        int rand_start = warp_rand()%tree_a.len;
        int curr_indx = rand_start;
        int s_ent = rand_start;
        u8 selected_depth = 0;
        do{
            u8 curr_depth = tree_get_exit_depth(banjo_tree,&(tree_a.pool[curr_indx]->me));
            if(curr_depth < oneway_depth){
                s_ent = curr_indx;
                break;
            }
            if(curr_depth < selected_depth){
                selected_depth = curr_depth;
                s_ent = curr_indx;
            }
            curr_indx++;
            curr_indx = curr_indx%tree_a.len;
        }while(curr_indx != rand_start);

        D_PRINTF("%3d: attaching [%02x]{%02x,%02x} <-> [%02x]{%02x,%02x} :\n",++i,
            s_exit, nodePool_a.pool[s_exit]->me.map, nodePool_a.pool[s_exit]->me.exit,
            s_ent, tree_a.pool[s_ent]->me.map,  tree_a.pool[s_ent]->me.exit);
        //make connection
        exitLUT_set(&(nodePool_a.pool[s_exit]->ret), &(tree_a.pool[s_ent]->me));
        exitLUT_set(&(tree_a.pool[s_ent]->ret), &(tree_a.pool[s_exit]->me));
        //delete exits
        nodePool_a.pool[s_exit] = nodePool_a.pool[--nodePool_a.len];
        tree_a.pool[s_ent] = tree_a.pool[--tree_a.len];
    }
    D_PRINTF("    POOL u=%3d a=%3d : TREE u=%3d a=%3d\n",
            nodePool_u.len, nodePool_a.len,
            tree_u.len, tree_a.len);
    


    //if something's not connected
    if(tree_u.len){
        for(int j = tree_u.len-1; j >= 0 ; j--){
            tree_a.pool[tree_a.len++] = tree_u.pool[j];
            tree_u.pool[j] = tree_u.pool[--tree_u.len];
        }
    }
    if(nodePool_a.len){
        for(int j = nodePool_a.len-1; j >= 0 ; j--){
            tree_a.pool[tree_a.len++] = nodePool_a.pool[j];
            nodePool_a.pool[j] = nodePool_a.pool[--nodePool_a.len];
        }
    }
    if(nodePool_u.len){
        for(int j = nodePool_u.len-1; j >= 0 ; j--){
            tree_a.pool[tree_a.len++] = nodePool_u.pool[j];
            nodePool_u.pool[j] = nodePool_u.pool[--nodePool_u.len];
        }
    }
    if(tree_a.len){
        D_PRINTF("SOMETHING IS MISSING!!!\n");
    }
    while(tree_a.len != 0){
        int rand1 = warp_rand()%tree_a.len;
        int rand2 = warp_rand()%tree_a.len;
        exitLUT_set(&(tree_a.pool[rand1]->ret), &(tree_a.pool[rand2]->me));
        exitLUT_set(&(tree_a.pool[rand2]->ret), &(tree_a.pool[rand1]->me));
        D_PRINTF("%3d: attaching [%02x]{%02x,%02x} <-> [%02x]{%02x,%02x} :\n",++i,
            rand1, tree_a.pool[rand1]->me.map, tree_a.pool[rand1]->me.exit,
            rand2, tree_a.pool[rand2]->me.map, tree_a.pool[rand2]->me.exit);
        if(rand1 == rand2){
            tree_a.pool[rand1] = tree_a.pool[--tree_a.len];
        }
        else{
            if(rand1 == tree_a.len - 1){
                tree_a.pool[rand1] = tree_a.pool[--tree_a.len];
                tree_a.pool[rand2] = tree_a.pool[--tree_a.len];
            }else{
                tree_a.pool[rand2] = tree_a.pool[--tree_a.len];
                tree_a.pool[rand1] = tree_a.pool[--tree_a.len];
            }
        }   
    }
    tree_print(banjo_tree);


    // TODO attach all one way and inaccessible nodes s.t.
        //{x,y},{a,b} INACCESSIBLE -> futher exits 
        //{x,y},{a,NULL} -> further exits
        //{x,NULL},{a,b} -> closer exit
    

    // TODO select pairs of loadzones s.t. no critical nodes are violated

    // TODO loop final exits on self

    //close off open tree exits (logic for attaching to nodes. also fine if node attaches to self (deadend)

    return;
}

/*
const warp_t vanilla_pairs[] = {

    // SM,sm
    //todo handle SM differently
    {{0x01,0x12}, {0x8C,0x01}, 0, 0}, //SM,sm <- SM,banjo_house
    {{0x01,0x13}, {0x69,0x12}, 0, 0}, //SM,sm <- lair,mm_loby

    //MM,mm
    {{0x02,0x01}, {0x0E,0x01}, 0, 0}, //MM,mm <- MM,mm_mumbos
    {{0x02,0x02}, {0x0C,0x02}, 0, 0}, //MM,mm <- MM,mm_tickers_lower
    {{0x02,0x03}, {0x0C,0x01}, 0, WARP_FLAG_INACCESSIBLE}, //MM,mm <- MM,mm_tickers_upper
    //{{0x02,0x04}, {,}, WARP_FLAG_ONEWAY}, //MM,witchswitch
    {{0x02,0x04}, {0x69,0x02}, 0, 0}, //MM,mm <- lair,mm_lobby

    //TTC,blubber_ship
    {{0x05, 0x05}, {0x07,0x06}, 0, 0}, //TTC,blubber_ship_top <- TTC,ttc
    {{0x05, 0x06}, {0x07,0x07}, 0, 0}, //TTC,blubber_ship_bottom <- TTC,ttc

    //TTC,nipper_shell
    {{0x06, 0x01}, {0x07,0x0A}, 0, 0}, //TTC, nipper_shell <- TTC,ttc

    //TTC, ttc
    {{0x07,0x03}, {0x0A, 0x01}, 0, 0}, //TTC,ttc <- TTC,sandcastle
    {{0x07,0x04}, {0x6D, 0x04}, 0, 0}, //TTC,ttc <- lair,ttc_lobby
    {{0x07,0x06}, {0x05, 0x05}, 0, WARP_FLAG_BEAK_BUST | WARP_FLAG_EGGS}, //TTC,ttc <- TTC,blubber_ship_top
    {{0x07,0x07}, {0x05, 0x06}, 0, 0},      //TTC,ttc <- TTC,blubber_ship_bottom
    {{0x07,0x08}, {0x07, 0x0C}, 0, WARP_FLAG_FLIGHT },      //TTC,lighthouse_top <- TTC,lighthouse_bottom
    {{0x07,0x0A}, {0x06, 0x01}, 0, 0}, //TTC,ttc <- TTC,nipper_shell
    {{0x07,0x0C}, {0x07, 0x08}, 0, WARP_FLAG_FLIGHT },      //TTC,stairs_top <- TTC,stairs_bottom
    {{0x07,0x0E}, {0x07, 0x0F}, 0, WARP_FLAG_FLIGHT },      //TTC,stairs_bottom <- TTC,stairs_top
    {{0x07,0x0F}, {0x07, 0x0E}, 0, WARP_FLAG_FLIGHT },      //TTC,lighthouse_bottom <- TTC,lighthouse_top
    //0x14 witch_switch
    //0x80 sharkfood_island


    //TTC, sandcastle
    {{0x0A, 0x01}, {0x07,0x03}, 0, 0}, //TTC,sandcastle <- TTC,ttc

    //CC, cc
    {{0x0B, 0x01}, {0x22,0x07}, WARP_FLAG_EGGS, 0}, //CC,cc <- CC,inside_left_tooth
    {{0x0B, 0x02}, {0x22,0x06}, WARP_FLAG_EGGS, 0}, //CC,cc <- CC,inside_right_tooth
    {{0x0B, 0x03}, {0x22,0x05}, 0, 0}, //CC,cc <- CC,inside_left_gil
    {{0x0B, 0x04}, {0x22,0x04}, 0, 0}, //CC,cc <- CC,inside_right_gil
    {{0x0B, 0x05}, {0x70,0x02}, 0, 0}, //CC,cc <- lair,cc_lobby
    {{0x0B, NULL}, {0x21,0x01}, WARP_FLAG_ONEWAY, 0}, ////CC,witch_switch_room <- CC,cc


    //MM,mm_tickers
    {{0x0C,0x02}, {0x02,0x02}, 0, 0},     //MM,mm_tickers_lower <- MM,mm 
    {{0x0C,0x01}, {0x02,0x03}, 0, WARP_FLAG_TERMITE | WARP_FLAG_BEE | WARP_FLAG_CROC | WARP_FLAG_WALRUS | WARP_FLAG_PUMPKIN},  //MM,mm_tickers_upper <- MM,mm 

    //BGS, bgs
    {{0x0D,0x02}, {0x72,0x02}, 0, 0},     //BGS,bgs <- lair,bgs_lobby
    {{0x0D,0x03}, {0x11,0x01}, WARP_FLAG_BEAK_BUST, 0},     //BGS,bgs <- BGS,tanktup
    {{0x0D,0x04}, {0x10,0x04}, WARP_FLAG_CROC, 0},     //BGS,bgs <- BGS,vile_right  
    {{0x0D,0x05}, {0x10,0x05}, WARP_FLAG_CROC, 0},     //BGS,bgs <- BGS,vile_left  
    {{0x0D,0x06}, {0x47,0x01}, WARP_FLAG_WADING_BOOTS, 0},     //BGS,bgs <- BGS,bgs_mumbos  
    //0x14 witchswitch

    //MM,mm_mumbos
    {{0x0E,0x01}, {0x02,0x01}, 0, 0}, //MM,mm_mumbos <- MM,mm

    //BGS, vile
    {{0x10,0x04}, {0x0D,0x04}, WARP_FLAG_CROC, 0},     //BGS,vile_right <- BGS,bgs   
    {{0x10,0x05}, {0x0D,0x05}, WARP_FLAG_CROC, 0},     //BGS,vile_left <- BGS,bgs  

    //BGS,tanktup
    {{0x11,0x01}, {0x0D,0x03}, 0, 0},     //BGS,tanktup <- BGS,bgs  

    //GV,gv
    {{0x12,0x02}, {0x1A,0x01}, WARP_FLAG_EGGS, 0},      //GV,gv <- GV,jinxy
    {{0x12,0x03}, {0x13,0x01}, WARP_FLAG_BEAK_BUST, 0}, //GV,gv <- GV,matching_game
    {{0x12,0x04}, {0x14,0x01}, WARP_FLAG_EGGS, 0},      //GV,gv <- GV,sandybutt_front
    {{0x12,0x05}, {0x15,0x06}, WARP_FLAG_GV_WATER, 0},  //GV,gv <- GV,water_pyramid
    {{0x12,0x06}, {0x16,0x07}, WARP_FLAG_BEAK_BOMB | WARP_FLAG_FLIGHT, 0},  //GV,gv <- GV,rubee
    {{0x12,0x07}, {0x14,NULL}, WARP_FLAG_ONEWAY, 0},    //GV,gv <- GV,sandybutt_back 
    {{0x12,0x08}, {0x6E,0x03}, 0, 0},      //GV,gv <- lair, GV_Lobby
    {{0x12,NULL}, {0x15,0x02}, 0, WARP_FLAG_TURBO_TALON | WARP_FLAG_FLIGHT},    //GV,gv <- GV,water_pyramid_top 
    //0xA sns chamber

    //GV,matching_game
    {{0x13,0x01}, {0x12,0x03}, 0, 0}, //GV,matching_game <- GV,gv 

    //GV,sandybutt
    {{0x14,0x01}, {0x12,0x04}, 0, 0}, //sandybutt_front <- GV,gv 
    {{0x14,NULL}, {0x12,0x07}, WARP_FLAG_ONEWAY, 0}, //GV,sandybutt_back <- GV,gv  

    //GV,water_pyramid
    //0x01 cutscene return
    {{0x15,0x02}, {0x12,NULL}, WARP_FLAG_ONEWAY, 0}, //GV,water_pyramid_top <- GV,gv
    {{0x15,0x06}, {0x12,0x05}, 0, 0}, //GV,water_pyramid_front <- GV,gv
    
    //GV,rubee
    {{0x16,0x07}, {0x12,0x06},  0, 0}, //GV,rubee <- GV,gv

    //GV,jinxy
    {{0x1A,0x01}, {0x12,0x02}, 0, 0}, //GV,jinxy <- GV,gv

    //MMM,mmm
    {{0x1B,0x01}, {0x26,0x01}, 0, 0}, //MMM,mmm <- MMM,dining_room_front
    {{0x1B,NULL}, {0x26,0x02}, WARP_FLAG_ONEWAY, 0}, //MMM,dining_room_chimney 
    {{0x1B,0x03}, {0x25,0x01}, 0, 0}, //MMM,mmm <- MMM,well_top
    {{0x1B,0x04}, {0x24,0x01}, 0, 0}, //MMM,mmm <- MMM,tumblar
    {{0x1B,0x05}, {0x1C,0x01}, WARP_FLAG_BEAK_BUST | WARP_FLAG_TURBO_TALON, 0}, //MMM,mmm <- MMM,church
    {{0x1B,0x06}, {0x2B,0x01}, 0, 0}, //MMM,mmm <- MMM,church_secret
    {{0x1B,0x08}, {0x2F,0x02}, WARP_FLAG_PUMPKIN, 0}, //MMM,mmm <- MMM,rain_barrel_bottom
    {{0x1B,NULL}, {0x2F,0x01}, WARP_FLAG_PUMPKIN | WARP_FLAG_ONEWAY, 0}, //MMM,rain_barrel_top
    {{0x1B,0x09}, {0x1D,0x01}, 0, 0}, //MMM,mmm <- MMM,cellar
    {{0x1B,0x0A}, {0x2A,0x01}, 0, 0}, //MMM,mmm <- MMM,red_feather_room
    {{0x1B,0x0B}, {0x28,0x01}, 0, 0}, //MMM,mmm <- MMM,blue_egg_room
    {{0x1B,0x0C}, {0x2C,0x01}, 0, 0}, //MMM,mmm <- MMM,bathroom
    {{0x1B,0x0D}, {0x2E,0x01}, 0, 0}, //MMM,mmm <- MMM,honey_carrier_room
    {{0x1B,0x0E}, {0x2D,0x01}, 0, 0}, //MMM,mmm <- MMM,bedroom
    {{0x1B,0x0F}, {0x29,0x01}, 0, 0}, //MMM,mmm <- MMM,music_note_room
    {{0x1B,0x10}, {0x1B,0x11}, 0, 0}, //MMM,mmm_clock_bottom <- MMM,mmm_clock_top
    {{0x1B,0x11}, {0x1B,0x10}, 0, WARP_FLAG_INACCESSIBLE | WARP_FLAG_BEE}, //MMM,mmm_clock_top <- MMM,mmm_clock_bottom
    {{0x1B,0x12}, {0x30,0x01}, 0, 0}, //MMM,mmm <- MMM,mmm_mumbos
    {{0x1B,0x13}, {0x25,0x04}, WARP_FLAG_PUMPKIN, 0}, //MMM,mmm <- MMM,well_bottom
    {{0x1B,0x14}, {0x75,0x02}, 0, 0}, //MMM,mmm <- lair,mmm_lobby

    //MMM,church
    {{0x1C,0x01}, {0x1B,0x05}, 0, 0}, //MMM,church <- MMM,mmm

    //MMM, cellar
    {{0x1D,0x01}, {0x1B,0x09}, 0, 0}, //MMM,cellar <- MMM,mmm

    //CC, witch_switch_room
    {{0x21,0x01}, {0x0B,NULL}, WARP_FLAG_ONEWAY, 0}, //CC,witch_switch_room <- CC,cc
    {{0x21,NULL}, {0x22,0x01}, WARP_FLAG_ONEWAY, 0}, // CC,inside_jiggy
    {{0x21,NULL}, {0x22,0x02}, WARP_FLAG_ONEWAY, 0}, // CC,inside_ws

    //CC, inside
    {{0x22,0x01}, {0x21,NULL}, WARP_FLAG_ONEWAY, 0}, //CC,inside <-CC,ws_jiggy
    {{0x22,0x02}, {0x21,NULL}, WARP_FLAG_ONEWAY, 0},  //CC,inside <-CC,ws_ws
    {{0x22,0x03}, {0x23,0x01}, WARP_FLAG_FLIGHT, 0},  //CC,inside <-CC,wonderwing
    {{0x22,0x04}, {0x0B,0x04}, 0, 0},         //CC,inside_right_gil       <- CC,cc
    {{0x22,0x05}, {0x0B,0x03}, 0, 0},         //CC,inside_left_gil       <- CC,cc
    {{0x22,0x06}, {0x0B,0x02}, WARP_FLAG_INACCESSIBLE, 0}, //CC,inside_right_tooth    <- CC,cc
    {{0x22,0x07}, {0x0B,0x01}, WARP_FLAG_INACCESSIBLE, 0}, //CC,inside_left_tooth     <- CC,cc
    
    //CC,wonderwing
    {{0x23,0x01}, {0x22,0x03}, 0, 0},  //CC,wonderwing <- CC,inside

    //MMM,tumblar
    {{0x24,0x01}, {0x1B,0x04}, 0, 0}, //MMM,tumblar <- MMM,mmm 

    //MMM,well_top
    {{0x25,0x01}, {0x1B,0x03}, 0, 0}, //MMM,well_top <- MMM,mmm
    {{0x25,0x04}, {0x1B,0x13}, WARP_FLAG_PUMPKIN, 0}, //MMM,well_bottom <- MMM,mmm

    //MMM,dining_room
    {{0x26,0x01}, {0x1B,0x01}, 0, 0},     //MMM,dining_room_front <- MMM,mmm 
    {{0x26,0x02}, {0x1B,NULL}, WARP_FLAG_ONEWAY, 0},     //MMM,dining_room_chimney <- MMM,mmm 

    //FP,fp
    {{0x27,0x01}, {0x6F,0x06}, WARP_FLAG_TALON_TROT, 0}, //FP,fp <- FP,lobby 
    {{0x27,0x06}, {0x7F,0x01}, WARP_FLAG_WALRUS, 0},     //FP,fp <- FP,wozza 
    {{0x27,0x07}, {0x48,0x01}, 0, 0}, //FP,fp <- FP,mumbos
    {{0x27,0x08}, {0x41,0x01}, WARP_FLAG_TALON_TROT, 0}, //FP,fp <- FP,igloo 
    {{0x27,0x09}, {0x53,0x01}, 0, 0}, //FP,fp <- FP,tree 

    //MMM,blue_egg_room
    {{0x28,0x01}, {0x1B,0x0B}, 0, 0}, //MMM,blue_egg_room <- MMM,mmm

    //MMM,music_note_room
    {{0x29,0x01}, {0x1B,0x0F}, 0, 0}, //MMM,music_note_room <- MMM,mmm

    //MMM,red_feather_room
    {{0x2A,0x01}, {0x1B,0x0A}, 0, 0}, //MMM,red_feather_room <- MMM,mmm 

    //MMM,chruch_secret
    {{0x2B,0x01}, {0x1B,0x06}, 0, 0}, //MMM,church_secret <- MMM,mmm

    //MMM,bathroom
    {{0x2C,0x01}, {0x1B,0x0C}, 0, 0}, //MMM,bathroom <- MMM,mmm
    {{0x2C,0x04}, {0x8D,0x04}, WARP_FLAG_PUMPKIN, 0}, //MMM,bathroom <- MMM,logo

    //MMM,bedroom
    {{0x2D,0x01}, {0x1B,0x0E}, 0, 0}, //MMM,bedroom <- MMM,mmm

    //MMM,honey_carrier_room
    {{0x2E,0x01}, {0x1B,0x0D}, 0, 0}, //MMM,honey_carrier_room <- MMM,mmm

    //MMM,rain_barrel
    {{0x2F,0x01}, {0x1B,NULL}, WARP_FLAG_ONEWAY, 0}, //MMM,mmm <- MMM,rain_barrel_top
    {{0x2F,0x02}, {0x1B,0x06}, 0, 0}, //MMM,mmm <- MMM,rain_barrel

    //MMM,mmm_mumbos
    {{0x30,0x01}, {0x1B,0x12}, 0, 0}, //MMM,mmm_mumbos <- MMM,mmm

    //RBB, rbb
    {{0x31,0x01}, {0x3F,0x01}, 0, 0}, //RBB,rbb <- RBB,capt_quarters
    {{0x31,0x02}, {0x39,0x01}, 0, 0}, //RBB,rbb <- RBB,crew_quarters
    {{0x31,0x03}, {0x34,0x04}, 0, 0}, //RBB,rbb <- RBB,engine_button
    {{0x31,0x04}, {0x3C,0x01}, 0, 0}, //RBB,rbb <- RBB,kitchen
    {{0x31,0x05}, {0x3D,0x01}, 0, 0}, //RBB,rbb <- RBB,navigation
    {{0x31,0x06}, {0x3B,0x01}, 0, 0}, //RBB,rbb <- RBB,hull
    {{0x31,0x07}, {0x34,0x01}, 0, 0}, //RBB,rbb <- RBB,engine_gears
    {{0x31,0x08}, {0x35,0x01}, 0, 0}, //RBB,rbb <- RBB,boat_house
    {{0x31,0x09}, {0x37,0x01}, 0, 0}, //RBB,rbb <- RBB,left_container
    {{0x31,0x0A}, {0x3E,0x01}, 0, 0}, //RBB,rbb <- RBB,middle_container
    {{0x31,0x0B}, {0x38,0x01}, 0, 0}, //RBB,rbb <- RBB,right_container
    {{0x31,0x0C}, {0x3A,0x01}, 0, 0}, //RBB,rbb <- RBB,boss_boombox
    {{0x31,0x0D}, {0x36,0x01}, 0, 0}, //RBB,rbb <- RBB,warehouse_front
    {{0x31,NULL}, {0x36,0x02}, WARP_FLAG_ONEWAY, 0}, //RBB,rbb <- RBB,warehouse_skylight
    {{0x31,0x10}, {0x77,0x02}, 0, 0}, //RBB,rbb <- lair,rbb_lobby
    {{0x31,0x13}, {0x8B,0x04}, 0, 0}, //RBB,rbb <- RBB,anchor_room

    //RBB,engine
    {{0x34,0x01}, {0x31,0x07}, 0, 0}, //RBB,engine_gears <- RBB,rbb
    {{0x34,0x04}, {0x31,0x03}, 0, 0}, //RBB,engine_button <- RBB,rbb 

    //RBB,boat_house
    {{0x35,0x01}, {0x31,0x08}, 0, 0}, //RBB,boat_house <- RBB,rbb 

    //RBB,warehouse
    {{0x36,0x01}, {0x31,0x0d}, 0, 0},   //RBB,warehouse_front <- RBB,rbb
    {{0x36,0x02}, {0x31,NULL}, WARP_FLAG_ONEWAY, 0}, //RBB,warehouse_skylight <- RBB,rbb

    //RBB,left_container
    {{0x37,0x01}, {0x31,0x09}, 0, 0}, //RBB,left_container <- RBB,rbb 

    //RBB, right_container
    {{0x38,0x01}, {0x31,0x0B}, 0, 0}, //RBB,right_container <- RBB,rbb

    //RBB,crew_quarters
    {{0x39,0x01}, {0x31,0x02}, 0, 0}, //RBB,crew_quarters <- RBB,rbb
    
    //RBB,boss_boombox
    {{0x3A,0x01}, {0x31,0x0C}, 0, 0}, //RBB,boss_boombox <- RBB,rbb

    //RBB,hull
    {{0x3B,0x01}, {0x31,0x06}, 0, 0}, //RBB,hull <- RBB,rbb 

    //RBB,kitchen
    {{0x3C,0x01}, {0x31,0x04}, 0, 0}, //RBB,kitchen <- RBB,rbb

    //RBB navigation
    {{0x3D,0x01}, {0x31,0x05}, 0, 0}, //RBB,navigation <- RBB,rbb 

    //RBB,middle_container
    {{0x3E,0x01}, {0x31,0x0A}, 0, 0}, //RBB,rbb <- RBB,middle_container

    //RBB,capt_quarters
    {{0x3F,0x01}, {0x31,0x01}, 0, 0}, //RBB,capt_quarters <- RBB,rbb

    //CCW,hub
    {{0x40,0x01}, {0x46,0x01}, WARP_FLAG_WINTER_OPEN, 0}, //CCW,hub <- CCW,winter
    {{0x40,0x02}, {0x43,0x01}, WARP_FLAG_SPRING_OPEN, 0}, //CCW,hub <- CCW,spring
    {{0x40,0x03}, {0x44,0x01}, WARP_FLAG_SUMMER_OPEN, 0}, //CCW,hub <- CCW,summer
    {{0x40,0x04}, {0x45,0x01}, WARP_FLAG_AUTUMN_OPEN, 0}, //CCW,hub <- CCW,autumn
    {{0x40,0x07}, {0x79,0x06}, 0, 0}, //CCW,hub <- lair,ccw_lobby

    //FP,igloo
    {{0x41,0x01}, {0x27,0x08}, 0, 0}, //FP,igloo <- FP,fp

    //CCW_Spring
    {{0x43,0x01}, {0x40,0x02}, 0, 0}, //CCW,spring <- CCW,hub
    {{0x43,0x04}, {0x5E,0x02}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,sp_nabnuts_window
    {{0x43,0x06}, {0x5A,0x01}, WARP_FLAG_BEE, 0}, //CCW,spring <- CCW,sp_zubbas
    {{0x43,0x07}, {0x5E,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,sp_nabnuts_door
    {{0x43,0x08}, {0x65,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,sp_top_room
    {{0x43,0x09}, {0x4A,0x01}, 0, 0}, //CCW,spring <- CCW,sp_mumbos

    //CCW_Summer
    {{0x44,0x01}, {0x40,0x03}, 0, 0}, //CCW,summer <- CCW,hub
    {{0x44,0x04}, {0x5F,0x02}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,su_nabnuts_window
    {{0x44,0x06}, {0x5B,0x02}, WARP_FLAG_BEAK_BUST, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,su_zubbas
    {{0x44,0x07}, {0x5F,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,su_nabnuts_door
    {{0x44,0x08}, {0x66,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,su_top_room
    {{0x44,0x09}, {0x4B,0x01}, 0, 0}, //CCW,spring <- CCW,su_mumbos

    //CCW_AUTUMN
    {{0x45,0x01}, {0x40,0x04}, 0, 0}, //CCW,autumn <- CCW,hub
    {{0x45,0x04}, {0x60,0x02}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,au_nabnuts_window
    {{0x45,0x05}, {0x5C,0x01}, WARP_FLAG_BEAK_BUST, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,autumn <- CCW,au_zubbas
    {{0x45,0x06}, {0x63,0x02}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,au_flooded_room
    {{0x45,0x07}, {0x60,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,au_nabnuts_door
    {{0x45,0x08}, {0x67,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,au_top_room
    {{0x45,0x09}, {0x4C,0x01}, 0, 0}, //CCW,autumn <- CCW,au_mumbos

    //CCW,winter
    {{0x46,0x01}, {0x40,0x01}, 0, 0}, //CCW,winter <- CCW,hub
    {{0x46,0x04}, {0x61,0x02}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_FLIGHT}, //CCW,spring <- CCW,au_nabnuts_window
    {{0x46,0x05}, {0x64,0x01}, WARP_FLAG_BEAK_BUST, WARP_FLAG_TALON_TROT | WARP_FLAG_FLIGHT}, //CCW,spring <- CCW,au_zubbas
    {{0x46,0x06}, {0x62,0x01}, 0, WARP_FLAG_FLIGHT}, //CCW,spring <- CCW,au_flooded_room
    {{0x46,0x08}, {0x68,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_FLIGHT}, //CCW,spring <- CCW,au_top_room
    {{0x46,0x09}, {0x4D,0x01}, 0, 0}, //CCW,autumn <- CCW,au_mumbos

    //BGS, bgs_mumbos
    {{0x47,0x01}, {0x0D,0x06}, 0, 0}, //BGS,bgs_mumbos <- BGS,bgs

    //FP, fp_mumbos
    {{0x48,0x01}, {0x27,0x07}, 0, 0}, //FP,fp <- FP,mumbos

    //CCW,sp_mumbos
    {{0x4A,0x01}, {0x43,0x09}, 0, 0}, //CCW,spring <- CCW,sp_mumbos
    //CCW,su_mumbos
    {{0x4B,0x01}, {0x44,0x09}, 0, 0}, //CCW,spring <- CCW,su_mumbos
    //CCW,au_mumbos
    {{0x4C,0x01}, {0x45,0x09}, 0, 0}, //CCW,spring <- CCW,au_mumbos
    //CCW,wi_mumbos
    {{0x4D,0x01}, {0x46,0x09}, 0, 0}, //CCW,spring <- CCW,au_mumbos

    //FP,tree
    {{0x53,0x01}, {0x27,0x09}, 0, 0}, //FP,tree <- FP,fp

    //CCW,zubbas
    {{0x5A,0x01}, {0x43,0x06}, WARP_FLAG_BEE, 0}, //CCW,sp_zubbas <- CCW,spring
    {{0x5B,0x02}, {0x44,0x06}, 0, 0}, //CCW,su_zubbas <- CCW,summmer
    {{0x5C,0x01}, {0x45,0x05}, 0, 0}, //CCW,au_zubbas <- CCW,autumn


    //CCW,sp_nabuts
    {{0x5E,0x01}, {0x43,0x07}, 0, 0}, //CCW,door <- CCW,spring
    {{0x5E,0x02}, {0x43,0x04}, 0, 0}, //CCW,window <- CCW,spring
    //CCW,su_nabuts
    {{0x5F,0x01}, {0x44,0x07}, 0, 0}, //CCW,door <- CCW,summer
    {{0x5F,0x02}, {0x44,0x04}, 0, 0}, //CCW,window <- CCW,summer
    //CCW,au_nabuts
    {{0x60,0x01}, {0x45,0x07}, 0, 0}, //CCW,door <- CCW,summer
    {{0x60,0x02}, {0x45,0x04}, 0, 0}, //CCW,window <- CCW,summer
    //CCW,wi_nabuts
    {{0x61,0x01}, {0x46,0x07}, 0, 0}, //CCW,door <- CCW,summer
    {{0x61,0x02}, {0x46,0x04}, 0, 0}, //CCW,window <- CCW,summer

    //CCW,wi_nabnut_hoard
    {{0x62,0x01}, {0x46,0x06}, 0, 0}, //CCW,au_flooded_room <- CCW,autumn

    //CCW,au_flooded_room
    {{0x63,0x01}, {0x45,0x06}, 0, 0}, //CCW,au_flooded_room <- CCW,autumn
    //CCW,wi_flooded_room
    {{0x64,0x01}, {0x46,0x05}, 0, 0}, //CCW,au_flooded_room <- CCW,autumn


    //CCW,sp_top_room
    {{0x65,0x01}, {0x43,0x08}, 0, 0}, //CCW,spring <- CCW,sp_top_room
    //CCW,su_top_room
    {{0x66,0x01}, {0x44,0x08}, 0, 0}, //CCW,summer <- CCW,su_top_room
    //CCW,au_top_room
    {{0x67,0x01}, {0x45,0x08}, 0, 0}, //CCW,autumn <- CCW,au_top_room
    //CCW,wi_top_room
    {{0x68,0x01}, {0x46,0x08}, 0, 0}, //CCW,autumn <- CCW,au_top_room

    //lair,mm_lobby
    {{0x69,0x01}, {0x6A,0x01}, WARP_FLAG_50_NOTES, 0}, //lair,mm_lobby <- lair,ttc_cc_puzzles 
    {{0x69,0x02}, {0x02,0x04}, WARP_FLAG_MM_OPEN, 0}, //lair,mm_lobby <- MM,mm 
    {{0x69,0x12}, {0x01,0x13}, 0,0}, //lair,mm_loby <- SM,sm

    //lair, ttc_cc_puzzles
    {{0x6A,0x01}, {0x69,0x01}, 0, 0},//ttc_cc_puzzles <- mm_lobby
    {{0x6A,0x02}, {0x6B,0x01}, 0, 0},//ttc_cc_puzzles <- ccw_puzzle
    //0x08 purple cauldron

    //lair,ccw_puzzle
    {{0x6B,0x01},{0x6A,0x02}, 0, 0},//ccw_puzzle <- ttc_cc_puzzles
    {{0x6B,0x02},{0x6C,0x01}, 0, 0},//ccw_puzzle <- red_cauldron_room
    {{0x6B,0x03},{0x6D,0x01}, 0, 0},//ccw_puzzle <- ttc_lobby
    {{0x6B,0x04},{0x70,0x01}, 0, 0},//ccw_puzzle <- cc_lobby
    {{0x6B,0x05},{0x71,0x02}, WARP_FLAG_180_NOTES | WARP_FLAG_TALON_TROT, 0},//ccw_puzzle <- 260_nd

    //lair,red_cauldron_room
    {{0x6C,0x01}, {0x6B,0x02}, 0, 0},//red_cauldron_room <- ccw_puzzle

    //lair,ttc_lobby
    {{0x6D,0x01},{0x6B,0x03}, 0, 0},//ttc_lobby <- ccw_puzzle
    {{0x6D,0x04},{0x07,0x04}, WARP_FLAG_TTC_OPEN, 0},//ttc_lobby <- TTC,ttc

    //lair, gv_lobby
    {{0x6E,0x01}, {0x71,0x01}, 0, 0},    //lair, GV_Lobby <- lair,260 note door
    {{0x6E,0x02}, {0x6F,0x01}, 0, 0},    //lair, GV_Lobby <- lair,fp_lobby
    {{0x6E,0x03}, {0x12,0x08}, WARP_FLAG_GV_OPEN, 0}, //lair, GV_Lobby <- GV,gv
    {{0x6E,NULL}, {0x71,0x04}, WARP_FLAG_ONEWAY|WARP_FLAG_BEAK_BUST|WARP_FLAG_SHOCK_SPRING, 0},//gv_lobby_vase - 260_nd 

    //lair, fp_lobby
    {{0x6F,0x01}, {0x6E,0x02}, 0, 0},    //lair,fp_lobby <- lair, GV_Lobby
    {{0x6F,0x02}, {0x76,0x01}, WARP_FLAG_450_NOTES, 0},    //lair,fp_lobby <- lair, 640_nd
    {{0x6F,0x05}, {0x73,0x02}, WARP_FLAG_350_NOTES, 0},    //lair,fp_lobby <- lair, gv_puzzle
    {{0x6F,0x06}, {0x27,0x01}, WARP_FLAG_FP_OPEN, 0},    //lair,fp_lobby <- FP,fp
    //0x08 purple_cauldron
    //0x09 green_cauldron

    //lair,cc_lobby
    {{0x70,0x01}, {0x6B, 0x04}, 0, 0}, //lair,cc_lobby <- CC,cc
    {{0x70,0x02}, {0x0B, 0x05}, WARP_FLAG_BEAK_BUST | WARP_FLAG_CC_OPEN, 0}, //lair,cc_lobby <- CC,cc

    //260 note door
    {{0x71,0x01}, {0x6E,0x01}, WARP_FLAG_SHOCK_SPRING | WARP_FLAG_260_NOTES, 0},//260_nd <- gv_lobby_260door
    {{0x71,0x02}, {0x6B,0x05}, 0, 0},//260_nd <- ccw_puzzle
    {{0x71,0x03}, {0x72,0x01}, 0, 0},//260_nd <- bgs_lobby
    {{0x71,0x04}, {0x6E,NULL}, WARP_FLAG_ONEWAY, 0},//260_nd <- gv_lobby_vase

    //lair,bgs_lobby
    {{0x72,0x01}, {0x71,0x03}, 0, 0},//bgs_lobby <- 260_nd 
    {{0x72,0x02}, {0x0D,0x02}, WARP_FLAG_BGS_OPEN, 0}, //lair,bgs_lobby <- BGS,bgs

    //lair,gv_puzzle
    {{0x74,0x01}, {0x6F,0x05}, 0, 0}, //lair,gv_puzzle <- lair,fp_lobby
    {{0x74,0x02}, {0x75,0x01}, 0, 0}, //lair,gv_puzzle <- lair,mmm_lobby

    //lair,mmm_lobby
    {{0x75,0x01}, {0x74,0x02}, 0, 0}, //lair,gv_puzzle <- lair,mmm_lobby
    {{0x75,0x02}, {0x1B,0x14}, WARP_FLAG_MMM_OPEN, 0}, //lair,mmm_lobby <- MMM,mmm
    {{0x75,0x03}, {0x7A,0x01}, WARP_FLAG_PUMPKIN, 0}, //lair,mmm_lobby <- MMM,crypt

    //lair, 640_nd
    {{0x76,0x01}, {0x6F,0x02}, 0, 0}, //lair, 640_nd <- lair,fp_lobby
    {{0x76,0x02}, {0x79,0x02}, WARP_FLAG_INACCESSIBLE, 0}, //lair, 640_nd <- lair,rbb_lobby
    {{0x76,0x03}, {0x79,0x01}, WARP_FLAG_640_NOTES | WARP_FLAG_WATER_LVL3, 0}, //lair, 640_nd <- lair,rbb_lobby 
    {{0x76,0x04}, {0x77,0x01}, 0, 0}, //lair, 640_nd <- lair,rbb_lobby

    //lair,rbb_lobby
    {{0x77,0x01}, {0x76,0x04}, 0, 0}, //lair, 640_nd <- lair,rbb_lobby
    {{0x77,0x02}, {0x31,0x10}, WARP_FLAG_RBB_OPEN | WARP_FLAG_WATER_LVL2, 0}, //lair,rbb_lobby <- RBB,rbb
    {{0x77,0x03}, {0x78,0x02}, WARP_FLAG_WATER_LVL3, 0}, //lair,rbb_lobby <- RBB,rbb_puzzle_upper
    {{0x77,0x04}, {0x78,0x01}, 0, 0}, //lair,rbb_lobby <- RBB,rbb_puzzle_mmm

    //rbb_mmm_puzzles
    //CHANCE TO SOFTLOCK (ALWAYS DESPAWN GRATE)
    {{0x78,0x01}, {0x77,0x03}, WARP_FLAG_WATER_LVL2, 0}, //lair,rbb_lobby <- RBB,rbb_puzzle_rbb
    {{0x78,0x02}, {0x77,0x04}, 0, 0}, //lair,rbb_lobby <- RBB,rbb_puzzle_mmm

    //lair,ccw_lobby
    {{0x79,0x01}, {0x76,0x03}, 0, 0}, //lair, ccw_lobby <- lair, 640_nd
    {{0x79,0x02}, {0x76,0x02}, 0, 0}, //lair,ccw_lobby <- lair, 640_nd_upper
    {{0x79,0x03}, {0x80,0x01}, WARP_FLAG_765_NOTES, 0}, //lair,ccw_lobby <- lair,ff_lobby
    {{0x79,0x06}, {0x40,0x07}, WARP_FLAG_CCW_OPEN | WARP_FLAG_TALON_TROT, 0}, //lair,ccw_lobby <- CCW,hub
    //0x08 red_cauldron

    //lair,crypt
    {{0x7A,0x01}, {0x75,0x03}, WARP_FLAG_PUMPKIN, 0}, //MMM,crypt <- lair,mmm_lobby 

    //FP,wozza_cave
    {{0x7F,0x01}, {0x27,0x06}, WARP_FLAG_WALRUS, 0},     //FP,fp <- FP,wozza 

    //lair,ff_lobby
    {{0x80,0x01}, {0x79,0x03}, 0, 0}, //lair, 640_nd_upper <- lair,ff_lobby
    {{0x80,0x02}, {0x8E,0x01}, 0, 0}, //lair, 640_nd_upper <- lair,ff_lobby

    //RBB,anchor_room
    {{0x8B,0x04}, {0x31,0x13}, 0, 0}, //RBB,anchor_room <- RBB,rbb

    //SM,banjo_house
    {{0x8C,0x01},{0x01,0x12}, 0, 0}, //SM,banjo_house <- SM,sm 

    //MMM,logo
    {{0x8D,0x04}, {0x2C,0x04}, WARP_FLAG_PUMPKIN, 0}, //MMM,logo <- MMM,bathroom

    //FF
    {{0x8E,0x01}, {0x80,0x02}, 0, 0}, //lair, ff <- lair,ff_lobby
    {{0x8E,0x05}, {0x93,0x05}, 0, 0}, //lair, ff <- lair,810_nd
    //yellow_cauldron

    //FIGHT
    //TODO handle seperate
    {{0x90,0x01}, {0x93,0x01}, WARP_FLAG_FIGHT, 0}, //lair,810_nd <- FIGHT

    //lair 810_nd
    //TODO this room sucks
    {{0x93,0x01}, {0x90,0x01}, WARP_FLAG_DOG_OPEN, 0}, //lair,810_nd <- FIGHT
    {{0x93,0x05}, {0x8E,0x05}, WARP_FLAG_810_NOTES, 0}, //lair,810_nd <- lair, ff
    //0x08 yellow cauldron

    {{0,0}, {0,0}, 0,0}
};
*/