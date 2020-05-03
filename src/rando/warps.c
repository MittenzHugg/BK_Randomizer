#include "warps.h"
#include <stdlib.h>
#include <stdbool.h>

#ifdef DEBUG
#define D_PRINTF(f_, ...) printf((f_), ##__VA_ARGS__)
#else
#define D_PRINTF(f_, ...) while(false)
#endif

const u16 hash_table_len = 337;
extern exitMap_t exitLUT[337] = {0};

static u64  achieved_flags = 0;
static u32  tree_includes_map_flag = 0;
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

static u8   tree_jiggy_cnt = 0;
static u8   tree_token_cnt = 0;
static u16  tree_note_cnt = 0;
static u32 nodePool_avail_len;
static u32 nodePool_unavail_len;
static u32 tree_avail_len;
static u32 tree_unavail_len;
static warp_t** nodePool_avail;
static warp_t** nodePool_unavail;
static warp_t** tree_avail;
static warp_t** tree_unavail;

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

warp_t nodePool_available_init[] ={
    //MM,mm
    {{0x02,0x01}, {0x0E,0x01}, 0, 0}, //MM,mm <- MM,mm_mumbos
    {{0x02,0x02}, {0x0C,0x02}, 0, 0}, //MM,mm <- MM,mm_tickers_lower
    {{0x02,0x04}, {0x69,0x02}, 0, 0}, //MM,mm <- lair,mm_lobby

    //TTC,blubber_ship
    {{0x05, 0x05}, {0x07,0x06}, 0, 0}, //TTC,blubber_ship_top <- TTC,ttc
    {{0x05, 0x06}, {0x07,0x07}, 0, 0}, //TTC,blubber_ship_bottom <- TTC,ttc
    
    //TTC,nipper_shell
    {{0x06, 0x01}, {0x07,0x0A}, 0, 0}, //TTC, nipper_shell <- TTC,ttc

    //TTC, ttc
    {{0x07,0x03}, {0x0A, 0x01}, 0, 0}, //TTC,ttc <- TTC,sandcastle
    {{0x07,0x04}, {0x6D, 0x04}, 0, 0}, //TTC,ttc <- lair,ttc_lobby
    {{0x07,0x07}, {0x05, 0x06}, 0, 0},      //TTC,ttc <- TTC,blubber_ship_bottom
    {{0x07,0x0A}, {0x06, 0x01}, 0, 0}, //TTC,ttc <- TTC,nipper_shell

    //TTC, sandcastle
    {{0x0A, 0x01}, {0x07,0x03}, 0, 0}, //TTC,sandcastle <- TTC,ttc

    //CC, cc
    {{0x0B, 0x03}, {0x22,0x05}, 0, 0}, //CC,cc <- CC,inside_left_gil
    {{0x0B, 0x04}, {0x22,0x04}, 0, 0}, //CC,cc <- CC,inside_right_gil
    {{0x0B, 0x05}, {0x70,0x02}, 0, 0}, //CC,cc <- lair,cc_lobby

    //MM,mm_tickers
    {{0x0C,0x02}, {0x02,0x02}, 0, 0},     //MM,mm_tickers_lower <- MM,mm 

    //BGS, bgs
    {{0x0D,0x02}, {0x72,0x02}, 0, 0},     //BGS,bgs <- lair,bgs_lobby

    //MM,mm_mumbos
    {{0x0E,0x01}, {0x02,0x01}, 0, 0}, //MM,mm_mumbos <- MM,mm

    //BGS,tanktup
    {{0x11,0x01}, {0x0D,0x03}, 0, 0},     //BGS,tanktup <- BGS,bgs  

    //GV,gv
    {{0x12,0x08}, {0x6E,0x03}, 0, 0},      //GV,gv <- lair, GV_Lobby

    //GV,matching_game
    {{0x13,0x01}, {0x12,0x03}, 0, 0}, //GV,matching_game <- GV,gv 

    //GV,sandybutt
    {{0x14,0x01}, {0x12,0x04}, 0, 0}, //sandybutt_front <- GV,gv 

    //GV,water_pyramid
    {{0x15,0x06}, {0x12,0x05}, 0, 0}, //GV,water_pyramid_front <- GV,gv

    //GV,rubee
    {{0x16,0x07}, {0x12,0x06},  0, 0}, //GV,rubee <- GV,gv

    //GV,jinxy
    {{0x1A,0x01}, {0x12,0x02}, 0, 0}, //GV,jinxy <- GV,gv

    //MMM,mmm
    {{0x1B,0x01}, {0x26,0x01}, 0, 0}, //MMM,mmm <- MMM,dining_room_front
    {{0x1B,0x03}, {0x25,0x01}, 0, 0}, //MMM,mmm <- MMM,well_top
    {{0x1B,0x04}, {0x24,0x01}, 0, 0}, //MMM,mmm <- MMM,tumblar
    {{0x1B,0x06}, {0x2B,0x01}, 0, 0}, //MMM,mmm <- MMM,church_secret
    {{0x1B,0x09}, {0x1D,0x01}, 0, 0}, //MMM,mmm <- MMM,cellar
    {{0x1B,0x0A}, {0x2A,0x01}, 0, 0}, //MMM,mmm <- MMM,red_feather_room
    {{0x1B,0x0B}, {0x28,0x01}, 0, 0}, //MMM,mmm <- MMM,blue_egg_room
    {{0x1B,0x0C}, {0x2C,0x01}, 0, 0}, //MMM,mmm <- MMM,bathroom
    {{0x1B,0x0D}, {0x2E,0x01}, 0, 0}, //MMM,mmm <- MMM,honey_carrier_room
    {{0x1B,0x0E}, {0x2D,0x01}, 0, 0}, //MMM,mmm <- MMM,bedroom
    {{0x1B,0x0F}, {0x29,0x01}, 0, 0}, //MMM,mmm <- MMM,music_note_room
    {{0x1B,0x10}, {0x1B,0x11}, 0, 0}, //MMM,mmm_clock_bottom <- MMM,mmm_clock_top
    {{0x1B,0x12}, {0x30,0x01}, 0, 0}, //MMM,mmm <- MMM,mmm_mumbos
    {{0x1B,0x14}, {0x75,0x02}, 0, 0}, //MMM,mmm <- lair,mmm_lobby

    //MMM,church
    {{0x1C,0x01}, {0x1B,0x05}, 0, 0}, //MMM,church <- MMM,mmm

    //MMM, cellar
    {{0x1D,0x01}, {0x1B,0x09}, 0, 0}, //MMM,cellar <- MMM,mmm

    //CC, inside
    {{0x22,0x04}, {0x0B,0x04}, 0, 0},         //CC,inside_right_gil       <- CC,cc
    {{0x22,0x05}, {0x0B,0x03}, 0, 0},         //CC,inside_left_gil       <- CC,cc

    //CC,wonderwing
    {{0x23,0x01}, {0x22,0x03}, 0, 0},  //CC,wonderwing <- CC,inside

    //MMM,tumblar
    {{0x24,0x01}, {0x1B,0x04}, 0, 0}, //MMM,tumblar <- MMM,mmm 

    //MMM,well_top
    {{0x25,0x01}, {0x1B,0x03}, 0, 0}, //MMM,well_top <- MMM,mmm

    //MMM,dining_room
    {{0x26,0x01}, {0x1B,0x01}, 0, 0},     //MMM,dining_room_front <- MMM,mmm 

    //FP,fp
    {{0x27,0x07}, {0x48,0x01}, 0, 0}, //FP,fp <- FP,mumbos
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

    //MMM,bedroom
    {{0x2D,0x01}, {0x1B,0x0E}, 0, 0}, //MMM,bedroom <- MMM,mmm

    //MMM,honey_carrier_room
    {{0x2E,0x01}, {0x1B,0x0D}, 0, 0}, //MMM,honey_carrier_room <- MMM,mmm

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
    {{0x31,0x10}, {0x77,0x02}, 0, 0}, //RBB,rbb <- lair,rbb_lobby
    {{0x31,0x13}, {0x8B,0x04}, 0, 0}, //RBB,rbb <- RBB,anchor_room

    //RBB,engine
    {{0x34,0x01}, {0x31,0x07}, 0, 0}, //RBB,engine_gears <- RBB,rbb
    {{0x34,0x04}, {0x31,0x03}, 0, 0}, //RBB,engine_button <- RBB,rbb 

    //RBB,boat_house
    {{0x35,0x01}, {0x31,0x08}, 0, 0}, //RBB,boat_house <- RBB,rbb 

    //RBB,warehouse
    {{0x36,0x01}, {0x31,0x0d}, 0, 0},   //RBB,warehouse_front <- RBB,rbb

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
    {{0x40,0x07}, {0x79,0x06}, 0, 0}, //CCW,hub <- lair,ccw_lobby

    //FP,igloo
    {{0x41,0x01}, {0x27,0x08}, 0, 0}, //FP,igloo <- FP,fp

    //CCW_Spring
    {{0x43,0x01}, {0x40,0x02}, 0, 0}, //CCW,spring <- CCW,hub
    {{0x43,0x09}, {0x4A,0x01}, 0, 0}, //CCW,spring <- CCW,sp_mumbos

     //CCW_Summer
    {{0x44,0x01}, {0x40,0x03}, 0, 0}, //CCW,summer <- CCW,hub
    {{0x44,0x09}, {0x4B,0x01}, 0, 0}, //CCW,spring <- CCW,su_mumbos

    //CCW_AUTUMN
    {{0x45,0x01}, {0x40,0x04}, 0, 0}, //CCW,autumn <- CCW,hub
    {{0x45,0x09}, {0x4C,0x01}, 0, 0}, //CCW,autumn <- CCW,au_mumbos

    //CCW,winter
    {{0x46,0x01}, {0x40,0x01}, 0, 0}, //CCW,winter <- CCW,hub
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
    {{0x69,0x02}, {0x02,0x04}, 0, 0}, //lair,mm_lobby <- MM,mm 
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

    //lair,red_cauldron_room
    {{0x6C,0x01}, {0x6B,0x02}, 0, 0},//red_cauldron_room <- ccw_puzzle

    //lair,ttc_lobby
    {{0x6D,0x01},{0x6B,0x03}, 0, 0},//ttc_lobby <- ccw_puzzle

    //lair, gv_lobby
    {{0x6E,0x01}, {0x71,0x01}, 0, 0},    //lair, GV_Lobby <- lair,260 note door
    {{0x6E,0x02}, {0x6F,0x01}, 0, 0},    //lair, GV_Lobby <- lair,fp_lobby

    //lair, fp_lobby
    {{0x6F,0x01}, {0x6E,0x02}, 0, 0},    //lair,fp_lobby <- lair, GV_Lobby

    //lair,cc_lobby
    {{0x70,0x01}, {0x6B, 0x04}, 0, 0}, //lair,cc_lobby <- CC,cc

    //260 note door
    {{0x71,0x02}, {0x6B,0x05}, 0, 0},//260_nd <- ccw_puzzle
    {{0x71,0x03}, {0x72,0x01}, 0, 0},//260_nd <- bgs_lobby

    //lair,bgs_lobby
    {{0x72,0x01}, {0x71,0x03}, 0, 0},//bgs_lobby <- 260_nd 

    //lair,gv_puzzle
    {{0x74,0x01}, {0x6F,0x05}, 0, 0}, //lair,gv_puzzle <- lair,fp_lobby
    {{0x74,0x02}, {0x75,0x01}, 0, 0}, //lair,gv_puzzle <- lair,mmm_lobby

    //lair,mmm_lobby
    {{0x75,0x01}, {0x74,0x02}, 0, 0}, //lair,gv_puzzle <- lair,mmm_lobby

    //lair, 640_nd
    {{0x76,0x01}, {0x6F,0x02}, 0, 0}, //lair, 640_nd <- lair,fp_lobby
    {{0x76,0x04}, {0x77,0x01}, 0, 0}, //lair, 640_nd <- lair,rbb_lobby

    //lair,rbb_lobby
    {{0x77,0x01}, {0x76,0x04}, 0, 0}, //lair, 640_nd <- lair,rbb_lobby
    {{0x77,0x04}, {0x78,0x01}, 0, 0}, //lair,rbb_lobby <- RBB,rbb_puzzle_mmm

    //rbb_mmm_puzzles
    //CHANCE TO SOFTLOCK (ALWAYS DESPAWN GRATE)
    {{0x78,0x02}, {0x77,0x04}, 0, 0}, //lair,rbb_lobby <- RBB,rbb_puzzle_mmm

    //lair,ccw_lobby
    {{0x79,0x01}, {0x76,0x03}, 0, 0}, //lair, ccw_lobby <- lair, 640_nd
    {{0x79,0x02}, {0x76,0x02}, 0, 0}, //lair,ccw_lobby <- lair, 640_nd_upper
    
    //lair,ff_lobby
    {{0x80,0x01}, {0x79,0x03}, 0, 0}, //lair, 640_nd_upper <- lair,ff_lobby
    {{0x80,0x02}, {0x8E,0x01}, 0, 0}, //lair, 640_nd_upper <- lair,ff_lobby

    //RBB,anchor_room
    {{0x8B,0x04}, {0x31,0x13}, 0, 0}, //RBB,anchor_room <- RBB,rbb

    //SM,banjo_house
    {{0x8C,0x01},{0x01,0x12}, 0, 0}, //SM,banjo_house <- SM,sm 

    //FF
    {{0x8E,0x01}, {0x80,0x02}, 0, 0}, //lair, ff <- lair,ff_lobby
    {{0x8E,0x05}, {0x93,0x05}, 0, 0}, //lair, ff <- lair,810_nd
    //yellow_cauldron
};

warp_t nodePool_unavailable_init[] ={
    //MM,mm
    {{0x02,0x03}, {0x0C,0x01}, 0, WARP_FLAG_INACCESSIBLE}, //MM,mm <- MM,mm_tickers_upper

    //TTC, ttc
    {{0x07,0x06}, {0x05, 0x05}, 0, WARP_FLAG_BEAK_BUST | WARP_FLAG_EGGS}, //TTC,ttc <- TTC,blubber_ship_top
    {{0x07,0x08}, {0x07, 0x0C}, 0, WARP_FLAG_FLIGHT },      //TTC,lighthouse_top <- TTC,lighthouse_bottom
    {{0x07,0x0C}, {0x07, 0x08}, 0, WARP_FLAG_FLIGHT },      //TTC,stairs_top <- TTC,stairs_bottom
    {{0x07,0x0E}, {0x07, 0x0F}, 0, WARP_FLAG_FLIGHT },      //TTC,stairs_bottom <- TTC,stairs_top
    {{0x07,0x0F}, {0x07, 0x0E}, 0, WARP_FLAG_FLIGHT },      //TTC,lighthouse_bottom <- TTC,lighthouse_top

    //CC, cc
    {{0x0B, 0x01}, {0x22,0x07}, WARP_FLAG_EGGS, 0}, //CC,cc <- CC,inside_left_tooth
    {{0x0B, 0x02}, {0x22,0x06}, WARP_FLAG_EGGS, 0}, //CC,cc <- CC,inside_right_tooth
    {{0x0B, NULL}, {0x21,0x01}, WARP_FLAG_ONEWAY, 0}, ////CC,witch_switch_room <- CC,cc

    //MM,mm_tickers
    {{0x0C,0x01}, {0x02,0x03}, 0, WARP_FLAG_TERMITE | WARP_FLAG_BEE | WARP_FLAG_CROC | WARP_FLAG_WALRUS | WARP_FLAG_PUMPKIN},  //MM,mm_tickers_upper <- MM,mm 

    //BGS, bgs
    {{0x0D,0x03}, {0x11,0x01}, WARP_FLAG_BEAK_BUST, 0},     //BGS,bgs <- BGS,tanktup
    {{0x0D,0x04}, {0x10,0x04}, WARP_FLAG_CROC, 0},     //BGS,bgs <- BGS,vile_right  
    {{0x0D,0x05}, {0x10,0x05}, WARP_FLAG_CROC, 0},     //BGS,bgs <- BGS,vile_left  
    {{0x0D,0x06}, {0x47,0x01}, WARP_FLAG_WADING_BOOTS, 0},     //BGS,bgs <- BGS,bgs_mumbos  

    //BGS, vile
    {{0x10,0x04}, {0x0D,0x04}, WARP_FLAG_CROC, 0},     //BGS,vile_right <- BGS,bgs   
    {{0x10,0x05}, {0x0D,0x05}, WARP_FLAG_CROC, 0},     //BGS,vile_left <- BGS,bgs  

    //GV,gv
    {{0x12,0x02}, {0x1A,0x01}, WARP_FLAG_EGGS, 0},      //GV,gv <- GV,jinxy
    {{0x12,0x03}, {0x13,0x01}, WARP_FLAG_BEAK_BUST, 0}, //GV,gv <- GV,matching_game
    {{0x12,0x04}, {0x14,0x01}, WARP_FLAG_EGGS, 0},      //GV,gv <- GV,sandybutt_front
    {{0x12,0x05}, {0x15,0x06}, WARP_FLAG_GV_WATER, 0},  //GV,gv <- GV,water_pyramid
    {{0x12,0x06}, {0x16,0x07}, WARP_FLAG_BEAK_BOMB | WARP_FLAG_FLIGHT, 0},  //GV,gv <- GV,rubee
    {{0x12,0x07}, {0x14,NULL}, WARP_FLAG_ONEWAY, 0},    //GV,gv <- GV,sandybutt_back 
    {{0x12,NULL}, {0x15,0x02}, 0, WARP_FLAG_TURBO_TALON | WARP_FLAG_FLIGHT},    //GV,gv <- GV,water_pyramid_top 

    //GV,sandybutt
    {{0x14,NULL}, {0x12,0x07}, WARP_FLAG_ONEWAY, 0}, //GV,sandybutt_back <- GV,gv  

    //GV,water_pyramid
    //0x01 cutscene return
    {{0x15,0x02}, {0x12,NULL}, WARP_FLAG_ONEWAY, 0}, //GV,water_pyramid_top <- GV,gv

    //MMM,mmm
    {{0x1B,NULL}, {0x26,0x02}, WARP_FLAG_ONEWAY, 0}, //MMM,dining_room_chimney 
    {{0x1B,0x05}, {0x1C,0x01}, WARP_FLAG_BEAK_BUST | WARP_FLAG_TURBO_TALON, 0}, //MMM,mmm <- MMM,church
    {{0x1B,0x08}, {0x2F,0x02}, WARP_FLAG_PUMPKIN, 0}, //MMM,mmm <- MMM,rain_barrel_bottom
    {{0x1B,NULL}, {0x2F,0x01}, WARP_FLAG_PUMPKIN | WARP_FLAG_ONEWAY, 0}, //MMM,rain_barrel_top
    {{0x1B,0x11}, {0x1B,0x10}, 0, WARP_FLAG_INACCESSIBLE | WARP_FLAG_BEE}, //MMM,mmm_clock_top <- MMM,mmm_clock_bottom
    {{0x1B,0x13}, {0x25,0x04}, WARP_FLAG_PUMPKIN, 0}, //MMM,mmm <- MMM,well_bottom

    //CC, inside
    {{0x22,0x01}, {0x21,NULL}, WARP_FLAG_ONEWAY, 0}, //CC,inside <-CC,ws_jiggy
    {{0x22,0x02}, {0x21,NULL}, WARP_FLAG_ONEWAY, 0},  //CC,inside <-CC,ws_ws
    {{0x22,0x03}, {0x23,0x01}, WARP_FLAG_FLIGHT, 0},  //CC,inside <-CC,wonderwing
    {{0x22,0x06}, {0x0B,0x02}, WARP_FLAG_INACCESSIBLE, 0}, //CC,inside_right_tooth    <- CC,cc
    {{0x22,0x07}, {0x0B,0x01}, WARP_FLAG_INACCESSIBLE, 0}, //CC,inside_left_tooth     <- CC,cc

    //MMM,well_top
    {{0x25,0x04}, {0x1B,0x13}, WARP_FLAG_PUMPKIN, 0}, //MMM,well_bottom <- MMM,mmm

    //MMM,dining_room
    {{0x26,0x02}, {0x1B,NULL}, WARP_FLAG_ONEWAY, 0},     //MMM,dining_room_chimney <- MMM,mmm 

    //FP,fp
    {{0x27,0x01}, {0x6F,0x06}, WARP_FLAG_TALON_TROT, 0}, //FP,fp <- FP,lobby 
    {{0x27,0x06}, {0x7F,0x01}, WARP_FLAG_WALRUS, 0},     //FP,fp <- FP,wozza
    {{0x27,0x08}, {0x41,0x01}, WARP_FLAG_TALON_TROT, 0}, //FP,fp <- FP,igloo 

    //MMM,bathroom
    {{0x2C,0x04}, {0x8D,0x04}, WARP_FLAG_PUMPKIN, 0}, //MMM,bathroom <- MMM,logo

    //MMM,rain_barrel
    {{0x2F,0x01}, {0x1B,NULL}, WARP_FLAG_ONEWAY, 0}, //MMM,mmm <- MMM,rain_barrel_top

    //RBB, rbb
    {{0x31,NULL}, {0x36,0x02}, WARP_FLAG_ONEWAY, 0}, //RBB,rbb <- RBB,warehouse_skylight

    //RBB,warehouse
    {{0x36,0x02}, {0x31,NULL}, WARP_FLAG_ONEWAY, 0}, //RBB,warehouse_skylight <- RBB,rbb

    //CCW,hub
    {{0x40,0x01}, {0x46,0x01}, WARP_FLAG_WINTER_OPEN, 0}, //CCW,hub <- CCW,winter
    {{0x40,0x02}, {0x43,0x01}, WARP_FLAG_SPRING_OPEN, 0}, //CCW,hub <- CCW,spring
    {{0x40,0x03}, {0x44,0x01}, WARP_FLAG_SUMMER_OPEN, 0}, //CCW,hub <- CCW,summer
    {{0x40,0x04}, {0x45,0x01}, WARP_FLAG_AUTUMN_OPEN, 0}, //CCW,hub <- CCW,autumn

    //CCW_Spring
    {{0x43,0x04}, {0x5E,0x02}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,sp_nabnuts_window
    {{0x43,0x06}, {0x5A,0x01}, WARP_FLAG_BEE, 0}, //CCW,spring <- CCW,sp_zubbas
    {{0x43,0x07}, {0x5E,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,sp_nabnuts_door
    {{0x43,0x08}, {0x65,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,sp_top_room

    //CCW_Summer
    {{0x44,0x04}, {0x5F,0x02}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,su_nabnuts_window
    {{0x44,0x06}, {0x5B,0x02}, WARP_FLAG_BEAK_BUST, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,su_zubbas
    {{0x44,0x07}, {0x5F,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,su_nabnuts_door
    {{0x44,0x08}, {0x66,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,su_top_room

    //CCW_AUTUMN
    {{0x45,0x04}, {0x60,0x02}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,au_nabnuts_window
    {{0x45,0x05}, {0x5C,0x01}, WARP_FLAG_BEAK_BUST, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,au_zubbas
    {{0x45,0x06}, {0x63,0x02}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,au_flooded_room
    {{0x45,0x07}, {0x60,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,au_nabnuts_door
    {{0x45,0x08}, {0x67,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,au_top_room

    //CCW,winter
    {{0x46,0x04}, {0x61,0x02}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_FLIGHT}, //CCW,spring <- CCW,au_nabnuts_window
    {{0x46,0x05}, {0x64,0x01}, WARP_FLAG_BEAK_BUST, WARP_FLAG_TALON_TROT | WARP_FLAG_FLIGHT}, //CCW,spring <- CCW,au_zubbas
    {{0x46,0x06}, {0x62,0x01}, 0, WARP_FLAG_FLIGHT}, //CCW,spring <- CCW,au_flooded_room

    //CCW,zubbas
    {{0x5A,0x01}, {0x43,0x06}, WARP_FLAG_BEE, 0}, //CCW,sp_zubbas <- CCW,spring

    //lair,mm_lobby
    {{0x69,0x01}, {0x6A,0x01}, WARP_FLAG_50_NOTES, 0}, //lair,mm_lobby <- lair,ttc_cc_puzzles 

    //lair,ccw_puzzle
    {{0x6B,0x05},{0x71,0x02}, WARP_FLAG_180_NOTES | WARP_FLAG_TALON_TROT, 0},//ccw_puzzle <- 260_nd

    //lair,ttc_lobby
    {{0x6D,0x04},{0x07,0x04}, WARP_FLAG_TTC_OPEN, 0},//ttc_lobby <- TTC,ttc

    //lair, gv_lobby
    {{0x6E,0x03}, {0x12,0x08}, WARP_FLAG_GV_OPEN, 0}, //lair, GV_Lobby <- GV,gv
    {{0x6E,NULL}, {0x71,0x04}, WARP_FLAG_ONEWAY|WARP_FLAG_BEAK_BUST|WARP_FLAG_SHOCK_SPRING, 0},//gv_lobby_vase - 260_nd 

    //lair, fp_lobby
    {{0x6F,0x02}, {0x76,0x01}, WARP_FLAG_450_NOTES, 0},    //lair,fp_lobby <- lair, 640_nd
    {{0x6F,0x05}, {0x73,0x02}, WARP_FLAG_350_NOTES, 0},    //lair,fp_lobby <- lair, gv_puzzle
    {{0x6F,0x06}, {0x27,0x01}, WARP_FLAG_FP_OPEN, 0},    //lair,fp_lobby <- FP,fp
    
    //lair,cc_lobby
    {{0x70,0x02}, {0x0B, 0x05}, WARP_FLAG_BEAK_BUST | WARP_FLAG_CC_OPEN, 0}, //lair,cc_lobby <- CC,cc

    //260 note door
    {{0x71,0x01}, {0x6E,0x01}, WARP_FLAG_SHOCK_SPRING | WARP_FLAG_260_NOTES, 0},//260_nd <- gv_lobby_260door
    {{0x71,0x04}, {0x6E,NULL}, WARP_FLAG_ONEWAY, 0},//260_nd <- gv_lobby_vase

    //lair,bgs_lobby
    {{0x72,0x02}, {0x0D,0x02}, WARP_FLAG_BGS_OPEN, 0}, //lair,bgs_lobby <- BGS,bgs

    //lair,mmm_lobby
    {{0x75,0x02}, {0x1B,0x14}, WARP_FLAG_MMM_OPEN, 0}, //lair,mmm_lobby <- MMM,mmm
    {{0x75,0x03}, {0x7A,0x01}, WARP_FLAG_PUMPKIN, 0}, //lair,mmm_lobby <- MMM,crypt

    //lair, 640_nd
    {{0x76,0x02}, {0x79,0x02}, WARP_FLAG_INACCESSIBLE, 0}, //lair, 640_nd <- lair,rbb_lobby
    {{0x76,0x03}, {0x79,0x01}, WARP_FLAG_640_NOTES | WARP_FLAG_WATER_LVL3, 0}, //lair, 640_nd <- lair,rbb_lobby 

    //lair,rbb_lobby
    {{0x77,0x02}, {0x31,0x10}, WARP_FLAG_RBB_OPEN | WARP_FLAG_WATER_LVL2, 0}, //lair,rbb_lobby <- RBB,rbb
    {{0x77,0x03}, {0x78,0x02}, WARP_FLAG_WATER_LVL3, 0}, //lair,rbb_lobby <- RBB,rbb_puzzle_upper

    //rbb_mmm_puzzles
    //CHANCE TO SOFTLOCK (ALWAYS DESPAWN GRATE)
    {{0x78,0x01}, {0x77,0x03}, WARP_FLAG_WATER_LVL2, 0}, //lair,rbb_lobby <- RBB,rbb_puzzle_rbb

    //lair,ccw_lobby
    {{0x79,0x03}, {0x80,0x01}, WARP_FLAG_765_NOTES, 0}, //lair,ccw_lobby <- lair,ff_lobby
    {{0x79,0x06}, {0x40,0x07}, WARP_FLAG_CCW_OPEN | WARP_FLAG_TALON_TROT, 0}, //lair,ccw_lobby <- CCW,hub

    //lair,crypt
    {{0x7A,0x01}, {0x75,0x03}, WARP_FLAG_PUMPKIN, 0}, //MMM,crypt <- lair,mmm_lobby 

    //FP,wozza_cave
    {{0x7F,0x01}, {0x27,0x06}, WARP_FLAG_WALRUS, 0},     //FP,fp <- FP,wozza 

    //MMM,logo
    {{0x8D,0x04}, {0x2C,0x04}, WARP_FLAG_PUMPKIN, 0}, //MMM,logo <- MMM,bathroom

    //lair 810_nd
    //TODO this room sucks
    {{0x93,0x01}, {0x90,0x01}, WARP_FLAG_DOG_OPEN, 0}, //lair,810_nd <- FIGHT
    {{0x93,0x05}, {0x8E,0x05}, WARP_FLAG_810_NOTES, 0}, //lair,810_nd <- lair, ff
    //0x08 yellow cauldron
};

warp_t tree_available_init[] ={
    // SM,sm
    {{0x01,0x12}, {0x8C,0x01}, 0, 0}, //SM,sm <- SM,banjo_house
    {{0x01,0x13}, {0x69,0x12}, 0, 0}, //SM,sm <- lair,mm_loby
};

warp_t tree_unavailable_init[] ={
    //only way to get these moves moved over

    //CC, witch_switch_room
    {{0x21,0x01}, {0x0B,NULL}, WARP_FLAG_ONEWAY, 0}, //CC,witch_switch_room <- CC,cc
    {{0x21,NULL}, {0x22,0x01}, WARP_FLAG_ONEWAY, 0}, // CC,inside_jiggy
    {{0x21,NULL}, {0x22,0x02}, WARP_FLAG_ONEWAY, 0}, // CC,inside_ws

    //FIGHT
    //TODO handle seperate
    {{0x90,0x01}, {0x93,0x01}, WARP_FLAG_FIGHT, 0}, //lair,810_nd <- FIGHT
};

void warp_mapping_init(void){
    nodePool_avail_len = 153;
    nodePool_avail = (warp_t**) malloc(nodePool_avail_len*sizeof(warp_t*));
    for(int i = 0; i < nodePool_avail_len; i++){
        nodePool_avail[i] = &(nodePool_available_init[i]);
    }

    nodePool_unavail_len = 92;
    nodePool_unavail = (warp_t**) malloc(nodePool_unavail_len*sizeof(warp_t*));
    for(int i = 0; i < nodePool_unavail_len; i++){
        nodePool_unavail[i] = &(nodePool_unavailable_init[i]);
    }

    tree_avail_len = 2;
    tree_avail = (warp_t**) malloc(tree_avail_len*sizeof(warp_t*));
    for(int i = 0; i < tree_avail_len; i++){
        tree_avail[i] = &(tree_available_init[i]);
    }

    tree_unavail_len = 4;
    tree_unavail = (warp_t**) malloc(tree_unavail_len*sizeof(warp_t*));
    for(int i = 0; i < tree_unavail_len; i++){
        tree_unavail[i] = &(tree_unavailable_init[i]);
    }

    tree_includes_map_flag = 0;
    achieved_flags = 0;
    tree_jiggy_cnt = 0;
    tree_token_cnt = 0;
    tree_note_cnt  = 0;
}

warp_t** warp_nodePool_randSelect(void){
    u32 np_i;
    u32 tree_i;
    warp_t* selected_nodePool;
    warp_t* selected_exit;
    if(tree_avail_len == 1){
        // TODO select poolnode with more than 1 available exit
        //
        return &nodePool_avail[rand()% nodePool_avail_len];
        return NULL;
    }
    else{
        return &nodePool_avail[rand()% nodePool_avail_len];
    }
}

void warp_attachWarps(warp_t** pool_node, warp_t** tree_node){
    //make connection
    exitLUT_set(&((*tree_node)->ret), &((*pool_node)->me));
    exitLUT_set(&((*pool_node)->ret), &((*tree_node)->me));

    u8 map_index = (*pool_node)->me.map;

    //delete nodePool warp
    *pool_node = nodePool_avail[--nodePool_avail_len];
    nodePool_avail = (warp_t**) realloc(nodePool_avail, sizeof(warp_t*)*nodePool_avail_len);
    
    //delete tree warp
    *tree_node = tree_avail[--tree_avail_len];
    tree_avail = (warp_t**) realloc(tree_avail, sizeof(warp_t*)*tree_avail_len);

    //move available exits
    for(int i = nodePool_avail_len-1; i >= 0; i--){
        if(nodePool_avail[i]->me.map == map_index){
            //increase tree_size
            tree_avail = (warp_t**) realloc(tree_avail, sizeof(warp_t*)*(++tree_avail_len));
            //copy exit to tree
            tree_avail[tree_avail_len-1] = nodePool_avail[i];
            //remove from nodePool
            nodePool_avail[i] = nodePool_avail[--nodePool_avail_len];
            nodePool_avail = (warp_t**) realloc(nodePool_avail, sizeof(warp_t*)*(nodePool_avail_len));
        }
    }

    //move unavailable exits
    for(int i = nodePool_unavail_len-1; i >= 0; i--){
        if(nodePool_unavail[i]->me.map == map_index){
            //increase tree_size
            tree_unavail = (warp_t**) realloc(tree_unavail, sizeof(warp_t*)*(++tree_unavail_len));
            //copy exit to tree
            tree_unavail[tree_unavail_len-1] = nodePool_unavail[i];
            //remove from nodePool
            nodePool_unavail[i] = nodePool_unavail[--nodePool_unavail_len];
            nodePool_unavail = (warp_t**) realloc(nodePool_unavail, sizeof(warp_t*)*(nodePool_unavail_len));
        }
    }
    
}

//MM

void warp_update_totals(u8 imap){
    //TOKENS
#pragma region TOKENS
    switch (imap)
    {
    case 0x05: //TTC, blubbers
    case 0x0C: //MM, tickers
    case 0x10: //BSG, vile
    case 0x11: //BSG, tanktup
    case 0x13: //GV, matching
    case 0x14: //GV, sandybutt
    case 0x15: //GV, water
    case 0x16: //GV, rubee
    case 0x1A: //GV, jinxy
    case 0x1D: //MMM, cellar
    case 0x22: //CC, inside
    case 0x25: //MMM, well
    case 0x26: //MMM, dining room
    case 0x2C: //MMM, bathroom
    case 0x2D: //MMM, bedroom
    case 0x38: //RBB, left container
    case 0x39: //RBB, crew quarters
    case 0x3B: //RBB, storeroom
    case 0x3C: //RBB, kitchen
    case 0x3D: //RBB, Navigation
    case 0x3E: //RBB, middle container 
    case 0x41: //FP, igloo
    case 0x47: //BGS, mumbos
    case 0x4B: //CCW, summer_mumbos
    case 0x61: //CCW, winter nabnut
    case 0x6A: //lair, ttc_cc_puzzle
    case 0x6B: //lair, ccw_puzzle
    case 0x6C: //lair, red_cauldron_room
    case 0x6E: //lair, GV_lobby
    case 0x6F: //lair, FP_lobby
    case 0x70: //lair, cc_lobby
    case 0x76: //lair, 640_nd
    case 0x77: //lair, rbb_lobby
    case 0x78: //lair, mmm_puzzle
    case 0x7A: //lair, crypt
        tree_token_cnt += 1;
        break;
    case 0x1C: //MMM, church
        tree_token_cnt += 2;
        break;
    case 0x34: //RBB, engine
        tree_token_cnt += 3;
        break;
    case 0x02: //MM, mm
    case 0x0B: //CC, cc
    case 0x46: //CCW, winter
        tree_token_cnt += 4;
        break;
    case 0x12: //GV, gv
    case 0x45: //CCW, autumn
        tree_token_cnt += 5;
        break;
    case 0x31: //RBB, rbb
    case 0x43: //CCW, spring
    case 0x44: //CCW, summer
        tree_token_cnt += 6;
        break;
    case 0x0D: //BGS, bgs
        tree_token_cnt += 7;
        break;
    case 0x1B: //MMM, mmm
        tree_token_cnt += 8;
        break;
    case 0x07: //TTC, ttc
    case 0x27: //FP, fp
        tree_token_cnt += 9;
        break;
    default:
        break;
    }
#pragma endregion

#pragma region NOTES
    // TODO NOTES
    switch (imap)
    {
    case 0x30: //MMM mumbos
        tree_note_cnt += 2;
        break;
    case 0x3F: //RBB capt qrts
    case 0x60: //CCW au_nabnut
        tree_note_cnt += 3;
        break;
    case 0x0A: //TTC sandcastle
    case 0x0E: //MM mumbos
    case 0x13: //GV matching
    case 0x15: //GV water
    case 0x1D: //MMM cellar
    case 0x24: //MMM tumblar
    case 0x2D: //MMM bedroom
    case 0x36: //RBB warehouse
    case 0x38: //RBB right_container
    case 0x39: //RBB crew quarters
    case 0x3D: //RBB navigation
    case 0x40: //CCW hub
    case 0x4C: //CCW au_mumbo
    case 0x5C: //CCW au_zubba
    case 0x8B: //RBB anchor
        tree_note_cnt += 4;
        break;
    case 0x2F: //MMM rainbarrel
    case 0x3B: //RBB hull
    case 0x3C: //RBB kitchen
        tree_note_cnt += 5;
        break;
    case 0x06: //TTC nipper
    case 0x0C: //TTC tickers
    case 0x10: //BGS vile
    case 0x11: //BGS tiptup
    case 0x21: //CC ws room
    case 0x23: //CC wonderwitn room
    case 0x48: //FP mumbo's
        tree_note_cnt += 6;
        break;
    case 0x14: //GV sandybutt
    case 0x1A: //GV jinxy
    case 0x25: //MMM well
        tree_note_cnt += 7;
        break;
    case 0x05: //TTC Blubber
    case 0x16: //GV rubee
    case 0x26: //MMM dining room
    case 0x37: //RBB left_container
        tree_note_cnt += 8;
        break;
    case 0x29: //MMM note door
        tree_note_cnt += 9;
        break;
    case 0x1C: //MMM church
        tree_note_cnt += 10;
        break;
    case 0x53: //FP xmas trees
        tree_note_cnt += 12;
        break;
    case 0x22: //CC insides
    case 0x34: //RBB engine
    case 0x43: //CCW spring
    case 0x44: //CCW summer
    case 0x46: //CCW winter
        tree_note_cnt += 16;
        break;
    case 0x47: //CCW autumn
        tree_note_cnt += 37;
        break;
    case 0x31: //RBB, rbb
        tree_note_cnt += 43;
        break;
    case 0x1B: //MMM, mmm
        tree_note_cnt += 47;
        break;
    case 0x12: //GV, gv
        tree_note_cnt += 70;
        break;
    case 0x0B: //CC,cc
        tree_note_cnt += 72;
        break;
    case 0x07: //TTC ttc
    case 0x27: //FP fp
        tree_note_cnt += 82;
        break;
    case 0x0D: //BGS bgs
        tree_note_cnt += 88;
        break;
    case 0x02: //MM, mm
        tree_note_cnt += 90;
        break;
    default:
        break;
    }
#pragma endregion

#pragma region JIGGIES
    // TODO JIGGIES
    switch (imap)
    {
    case 0x02: //MM, mm
        tree_jiggy_cnt += 10;
        break;
    case 0x05: //TTC, blubber
        tree_jiggy_cnt += 1;
        break;
    case 0x06: //TTC, nipper
        tree_jiggy_cnt += 1;
        break;
    case 0x07: //TTC, ttc
        tree_jiggy_cnt += 7;
        break;
    case 0x0A: //TTC, sandcastle
        tree_jiggy_cnt += 1;
        break;
    case 0x0B: //CC, cc
        tree_jiggy_cnt += 5;
        break;
    case 0x0D: //BGS,bgs
        tree_jiggy_cnt += 8;
        break;
    case 0x10: //BGS, vile
        tree_jiggy_cnt += 1;
        break;
    case 0x11: //BGS, tiptup
        tree_jiggy_cnt += 1;
        break;
    case 0x12: //GV, gv
        tree_jiggy_cnt += 5;
        break;
    case 0x13: //GV, matching
        tree_jiggy_cnt += 1;
        break;
    case 0x14: //GV, sandybutt
        tree_jiggy_cnt += 1;
        break;
    case 0x15: //GV, water
        tree_jiggy_cnt += 1;
        break;
    case 0x16: //GV, rubee
        tree_jiggy_cnt += 1;
        break;
    case 0x1A: //GV, jinxy
        tree_jiggy_cnt += 1;
        break;
    case 0x1B: //MMM, mmm
        tree_jiggy_cnt += 2;
        break;
    case 0x1C: //MMM, church
        tree_jiggy_cnt += 2;
        break;
    case 0x1D: //MMM, cellar
        tree_jiggy_cnt += 1;
        break;
    case 0x21: //CC, ws_room
        tree_jiggy_cnt += 1;
        break;
    case 0x22: //CC, inside
        tree_jiggy_cnt += 3;
        break;
    case 0x23: //CC, wonderwing
        tree_jiggy_cnt += 1;
        break;
    case 0x24: //MMM, tumblar
        tree_jiggy_cnt += 1;
        break;
    case 0x25: //MMM, well
        tree_jiggy_cnt += 1;
        break;
    case 0x26: //MMM, napper
        tree_jiggy_cnt += 1;
        break;
    case 0x27: //FP, fp
        tree_jiggy_cnt += 7;
        break;
    case 0x2F: //MMM, rain barrel
        tree_jiggy_cnt += 1;
        break;
    case 0x31: //RBB, rbb
        tree_jiggy_cnt += 3;
        break;
    case 0x34: //RBB, engine
        tree_jiggy_cnt += 2;
        break;
    case 0x36: //RBB, warehouse
        tree_jiggy_cnt += 1;
        break;
    case 0x3A: //RBB, boss_boombox
        tree_jiggy_cnt += 1;
        break;
    case 0x3E: //RBB, middle_container
        tree_jiggy_cnt += 1;
        break;
    case 0x3F: //RBB, captian quarters
        tree_jiggy_cnt += 1;
        break;
    case 0x41: //FP, igloo
        tree_jiggy_cnt += 1;
        break;
    case 0x44: //CCW, wummer
        tree_jiggy_cnt += 2;
        break;
    case 0x45: //CCW, wutumn
        tree_jiggy_cnt += 2;
        break;
    case 0x46: //CCW, winter
        tree_jiggy_cnt += 3;
        break;
    case 0x53: //FP, xmas
        tree_jiggy_cnt += 1;
        break;
    case 0x5A: //CCW, sp_zubba
        tree_jiggy_cnt += 1;
        break;
    case 0x5B: //CCW, su_zubba
        tree_jiggy_cnt += 1;
        break;
    case 0x68: //CCW, wi_toproom
        tree_jiggy_cnt += 1;
        break;
    case 0x69: //lair, mm_lobby
        tree_jiggy_cnt += 2;
        break;
    case 0x6A: //lair, cc_ws
        tree_jiggy_cnt += 1;
        break;
    case 0x6B: //lair, ccW_ws
        tree_jiggy_cnt += 1;
        break;
    case 0x6D: //lair, ttc_ws
        tree_jiggy_cnt += 1;
        break;
    case 0x6E: //lair, gv_ws
        tree_jiggy_cnt += 1;
        break;
    case 0x6F: //lair, fp_ws mm_ws
        tree_jiggy_cnt += 2;
        break;
    case 0x71: //lair, bgs_ws
        tree_jiggy_cnt += 1;
        break;
    case 0x76: //lair, rbb_ws
        tree_jiggy_cnt += 1;
        break;
    case 0x7F: //FP, wozza
        tree_jiggy_cnt += 1;
        break;
    case 0x8B: //RBB, anchor
        tree_jiggy_cnt += 1;
        break;
    case 0x8D: //MMM, loggo
        tree_jiggy_cnt += 1;
        break;
    default:
        break;
    }
#pragma endregion
}

void warp_update_flags(u8 imap){
    switch (imap)
    {
    case 0x02: //MM,mm
        achieved_flags |= WARP_FLAG_BEAK_BUST;
        achieved_flags |= WARP_FLAG_EGGS;
        achieved_flags |= WARP_FLAG_TALON_TROT;
        break;
    case 0x07: //TTC,ttc
        tree_includes_map_flag |= TREE_MAP_TTC_TTC;
        achieved_flags |= WARP_FLAG_SHOCK_SPRING;
        break;
    case 0x0D: //BGS,bgs
        achieved_flags |= WARP_FLAG_WADING_BOOTS;
        break;
    case 0x0E: //MM, mumbo
        tree_includes_map_flag |= TREE_MAP_MM_MUMBO;
        break;
    case 0x12: //GV,gv
        tree_includes_map_flag |= TREE_MAP_GV_GV;
        break;
    case 0x15: //GV, water_pyramid
        achieved_flags |= WARP_FLAG_GV_WATER;
        break;
    case 0x27: //FP, fp
        achieved_flags |= WARP_FLAG_BEAK_BOMB;
        break;
    case 0x30: //MMM, mumbo
        tree_includes_map_flag |= TREE_MAP_MMM_MUMBO;
        break;
    case 0x40: //CCW, hub
        tree_includes_map_flag |= TREE_MAP_CCW_HUB;
        break;
    case 0x43: //CCW, spring
        tree_includes_map_flag |= TREE_MAP_CCW_SPRING;
        break;
    case 0x44: //CCW, summer
        tree_includes_map_flag |= TREE_MAP_CCW_SUMMER;
        break;
    case 0x45: //CCW, autumn
        tree_includes_map_flag |= TREE_MAP_CCW_AUTUMN;
        break;
    case 0x47: //BGS, mumbo
        tree_includes_map_flag |= TREE_MAP_BGS_MUMBO;
        break;
    case 0x48: //FP, mumbo
        tree_includes_map_flag |= TREE_MAP_FP_MUMBO;
        break;
    case 0x4A: //CCW_sp, mumbo
        tree_includes_map_flag |= TREE_MAP_CCW_MUMBO;
        break;
    case 0x6A: //cc/ttc puzzles
        tree_includes_map_flag |= TREE_MAP_TTC_CC_PUZZLE;
        break;
    case 0x6B: //ccw puzzles
        tree_includes_map_flag |= TREE_MAP_CCW_PUZZLE;
        break;
    case 0x70: //bgs puzzle
        tree_includes_map_flag |= TREE_MAP_BGS_PUZZLE;
        break;
    case 0x72: //fp puzzle
        tree_includes_map_flag |= TREE_MAP_FP_PUZZLE;
        break;
    case 0x74: //gv puzzle
        tree_includes_map_flag |= TREE_MAP_GV_PUZZLE;
        break;
    case 0x77: //water switch 
        tree_includes_map_flag |= TREE_MAP_WS2;
        break;
    case 0x78: //mmm/rbb puzzle
        tree_includes_map_flag |= TREE_MAP_MMM_RBB_PUZZLE;
        break;
    case 0x7A: //water switch 1
        tree_includes_map_flag |= TREE_MAP_WS1;
        achieved_flags |= WARP_FLAG_PUMPKIN;
        break;
    case 0x90: //DoG
        tree_includes_map_flag |= TREE_MAP_DOG;
        break;
    default:
        break;
    }


    //update totals
    warp_update_totals(imap);

    
    // TODO tree_jiggy_cnt;
    // TODO tree_token_cnt_on_hand;
    // TODO tree_note_cnt;

    //MUMBOS
    if(!(achieved_flags & WARP_FLAG_TERMITE)){
        if((tree_token_cnt >= 5) && (tree_includes_map_flag & TREE_MAP_MM_MUMBO)){
            achieved_flags |= WARP_FLAG_TERMITE;
            tree_token_cnt -= 5;
        }
    }
    if(!(achieved_flags & WARP_FLAG_CROC)){
        if((tree_token_cnt >= 10) && (tree_includes_map_flag & TREE_MAP_BGS_MUMBO)){
            achieved_flags |= WARP_FLAG_CROC;
            tree_token_cnt -= 10;
        }
    }
    if(!(achieved_flags & WARP_FLAG_WALRUS)){
        if((tree_token_cnt >= 15) && (tree_includes_map_flag & TREE_MAP_FP_MUMBO)){
            achieved_flags |= WARP_FLAG_WALRUS;
            tree_token_cnt -= 15;
        }
    }
    if(!(achieved_flags & WARP_FLAG_PUMPKIN)){
        if((tree_token_cnt >= 20) && (tree_includes_map_flag & TREE_MAP_MMM_MUMBO)){
            achieved_flags |= WARP_FLAG_PUMPKIN;
            tree_token_cnt -= 20;
        }
    }
    if(!(achieved_flags & WARP_FLAG_BEE)){
        if((tree_token_cnt >= 25) && (tree_includes_map_flag & TREE_MAP_CCW_MUMBO)){
            achieved_flags |= WARP_FLAG_BEE;
            tree_token_cnt -= 25;
        }
    }

    //MOVES
    if(achieved_flags & WARP_FLAG_TALON_TROT){
        //flight
        if(tree_includes_map_flag & TREE_MAP_TTC_TTC)
            achieved_flags |= WARP_FLAG_FLIGHT;
        //turbo talon trainers
        if(tree_includes_map_flag & TREE_MAP_GV_GV)
            achieved_flags |= WARP_FLAG_TURBO_TALON;
    }
    if(achieved_flags & WARP_FLAG_BEAK_BUST){
        if(tree_includes_map_flag & TREE_MAP_CCW_HUB)
            achieved_flags |= WARP_FLAG_SPRING_OPEN;
        if((tree_includes_map_flag & TREE_MAP_CCW_SPRING) && (achieved_flags & (WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING)))
            achieved_flags |= WARP_FLAG_SUMMER_OPEN;
        if(tree_includes_map_flag & TREE_MAP_CCW_SUMMER)
            achieved_flags |= WARP_FLAG_AUTUMN_OPEN;
        if((tree_includes_map_flag & TREE_MAP_CCW_AUTUMN) && (achieved_flags & (WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING)))
            achieved_flags |= WARP_FLAG_WINTER_OPEN;
    }
   
    if(achieved_flags & WARP_FLAG_BEAK_BUST){
        //WATER SWITCHES
        if(tree_includes_map_flag & TREE_MAP_WS1)
            achieved_flags |= WARP_FLAG_WATER_LVL2;
         if(tree_includes_map_flag & TREE_MAP_WS2)
            achieved_flags |= WARP_FLAG_WATER_LVL3 | WARP_FLAG_WATER_LVL2;
    }

    //NOTE REQUIREMENTS
    if(tree_note_cnt >= 50){
        achieved_flags |= WARP_FLAG_50_NOTES;
    }
    else if(tree_note_cnt >= 180){
        achieved_flags |= WARP_FLAG_180_NOTES;
    }
    else if(tree_note_cnt >= 260){
        achieved_flags |= WARP_FLAG_260_NOTES;
    }
    else if(tree_note_cnt >= 350){
        achieved_flags |= WARP_FLAG_350_NOTES;
    }
    else if(tree_note_cnt >= 450){
        achieved_flags |= WARP_FLAG_450_NOTES;
    }
    else if(tree_note_cnt >= 640){
        achieved_flags |= WARP_FLAG_640_NOTES;
    }
    else if(tree_note_cnt >= 765){
        achieved_flags |= WARP_FLAG_765_NOTES;
    }
    else if(tree_note_cnt >= 810){
        achieved_flags |= WARP_FLAG_810_NOTES;
    }

    //OPEN LEVEL CONDITIONS
    if(tree_includes_map_flag & TREE_MAP_TTC_CC_PUZZLE){
        if((tree_jiggy_cnt >= 2) 
            && !(achieved_flags & WARP_FLAG_TTC_OPEN))
        {
            achieved_flags |= WARP_FLAG_TTC_OPEN;
            tree_jiggy_cnt -= 2;
        }
        if((tree_jiggy_cnt >= 5) 
            && (achieved_flags & WARP_FLAG_SHOCK_SPRING)
            && !(achieved_flags & WARP_FLAG_CC_OPEN))
        {
            achieved_flags |= WARP_FLAG_CC_OPEN;
            tree_jiggy_cnt -= 5;
        }
    }
    if(tree_includes_map_flag & TREE_MAP_BGS_PUZZLE){
        if((tree_jiggy_cnt >= 7) 
            && !(achieved_flags & WARP_FLAG_BGS_OPEN)
            && (achieved_flags & WARP_FLAG_BEAK_BUST))
        {
            achieved_flags |= WARP_FLAG_BGS_OPEN;
            tree_jiggy_cnt -= 7;
        }
    }
    if(tree_includes_map_flag & TREE_MAP_FP_PUZZLE){
        if((tree_jiggy_cnt >= 8) 
            && !(achieved_flags & WARP_FLAG_FP_OPEN)
            && (achieved_flags & WARP_FLAG_WADING_BOOTS))
        {
            achieved_flags |= WARP_FLAG_FP_OPEN;
            tree_jiggy_cnt -= 8;
        }
    }
    if(tree_includes_map_flag & TREE_MAP_GV_PUZZLE){
        if((tree_jiggy_cnt >= 9) 
            && !(achieved_flags & WARP_FLAG_GV_OPEN))
        {
            achieved_flags |= WARP_FLAG_GV_OPEN;
            tree_jiggy_cnt -= 9;
        }
    }
    if(tree_includes_map_flag & TREE_MAP_MMM_RBB_PUZZLE){
        if((tree_jiggy_cnt >= 10) 
            && !(achieved_flags & WARP_FLAG_MMM_OPEN))
        {
            achieved_flags |= WARP_FLAG_MMM_OPEN;
            tree_jiggy_cnt -= 10;
        }
        if((tree_jiggy_cnt >= 12) 
            && !(achieved_flags & WARP_FLAG_RBB_OPEN)
            && (achieved_flags & WARP_FLAG_WATER_LVL2))
        {
            achieved_flags |= WARP_FLAG_RBB_OPEN;
            tree_jiggy_cnt -= 12;
        }
    }
    if(tree_includes_map_flag & TREE_MAP_CCW_PUZZLE){
        if((tree_jiggy_cnt >= 15) 
            && !(achieved_flags & WARP_FLAG_CCW_OPEN))
        {
            achieved_flags |= WARP_FLAG_CCW_OPEN;
            tree_jiggy_cnt -= 15;
        }
    }
    if(tree_includes_map_flag & TREE_MAP_DOG){
        if((achieved_flags & WARP_FLAG_810_NOTES)){
            if((tree_jiggy_cnt >= 25) 
                && !(achieved_flags & WARP_FLAG_DOG_OPEN))
            {
                achieved_flags |= WARP_FLAG_DOG_OPEN;
                tree_jiggy_cnt -= 25;
            }
        }
    }
}

void warp_update_availability(void){
    //move unavailable nodePool exits
    for(int i = nodePool_unavail_len-1; i >= 0; i--){
        if(!(nodePool_unavail[i]->hard_flags & (~achieved_flags))
            && ( (nodePool_unavail[i]->soft_flags == 0) 
                || (nodePool_unavail[i]->soft_flags & achieved_flags))){
            //increase nodePool_avail size
            nodePool_avail = (warp_t**) realloc(nodePool_avail, sizeof(warp_t*)*(++nodePool_avail_len));
            //copy exit to nodePool_avail
            nodePool_avail[nodePool_avail_len-1] = nodePool_unavail[i];
            //remove from nodePool_unavail
            nodePool_unavail[i] = nodePool_unavail[--nodePool_unavail_len];
            nodePool_unavail = (warp_t**) realloc(nodePool_unavail, sizeof(warp_t*)*(nodePool_unavail_len));
        }
    }

    //move unavailable tree exits
    for(int i = tree_unavail_len-1; i >= 0; i--){
        if(!(tree_unavail[i]->hard_flags & (~achieved_flags))
            && ( (tree_unavail[i]->soft_flags == 0) 
                || (tree_unavail[i]->soft_flags & achieved_flags))){
            //increase tree_avail size
            tree_avail = (warp_t**) realloc(tree_avail, sizeof(warp_t*)*(++tree_avail_len));
            //copy exit to tree_avail
            tree_avail[tree_avail_len-1] = tree_unavail[i];
            //remove from tree_unavail
            tree_unavail[i] = tree_unavail[--tree_unavail_len];
            tree_unavail = (warp_t**) realloc(tree_unavail, sizeof(warp_t*)*(tree_unavail_len));
        }
    }
}

void wm_generate_mapping(u32 seed){
    exitLUT_clear();
    warp_mapping_init();
    srand(seed);


    int i = 0;
    while (nodePool_avail_len != 1){// not all nodes in tree
        //select exit from nodePool
        warp_t** selected_exit =  warp_nodePool_randSelect();
        //warp_t** selected_exit =  &nodePool_avail[1];
        if(selected_exit == NULL){break;} //error: unable to select final nodes without softlocking breaking for now
        u8 added_map = (*selected_exit)->me.map;

        //select exit from tree
        warp_t** selected_entrance = &tree_avail[rand()%tree_avail_len];
        //warp_t** selected_entrance = &tree_avail[0];

        //attach
        D_PRINTF("%3d: attaching {%02x,%02x} <-> {%02x,%02x} : ", ++i,
            (*selected_exit)->me.map, (*selected_exit)->me.exit,
            (*selected_entrance)->me.map, (*selected_entrance)->me.exit);

        warp_attachWarps(selected_exit, selected_entrance);

        warp_update_flags(added_map);
        warp_update_availability();

         D_PRINTF("POOL u=%3d a=%3d : TREE u=%3d a=%3d\n",
            nodePool_unavail_len, nodePool_avail_len,
            tree_unavail_len, tree_avail_len);

    };

    //make all unavailable nodes
    achieved_flags |= WARP_FLAG_FIGHT; //TODO: PLACE THIS AT MAX HEIGHT NODE
    achieved_flags |= WARP_FLAG_INACCESSIBLE;
    achieved_flags |= WARP_FLAG_ONEWAY;


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
    {{0x45,0x05}, {0x5C,0x01}, WARP_FLAG_BEAK_BUST, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,au_zubbas
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