#include <stdint.h>
#include "bk.h"
#include "sys.h"
#include "warps.h"
#include "simptypes.h"
#include "rando.h"

typedef struct exitMap{
    exit_t old;
    exit_t new;
} exitMap_t;

const u16 hash_table_len = 337; //about 1.3x exit count

static exitMap_t exitLUT[337] = {0};

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
static uint32_t nodePool_avail_len;
static uint32_t nodePool_unavail_len;
static uint32_t tree_avail_len;
static uint32_t tree_unavail_len;
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
    uint32_t np_i;
    uint32_t tree_i;
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

void warp_generate_mapping(void){
    exitLUT_clear();
    warp_mapping_init();
    srand(rando.current_seed);

    
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
        warp_attachWarps(selected_exit, selected_entrance);

        warp_update_flags(added_map);
        warp_update_availability();
    };

    //make all unavailable nodes
    //achieved_flags |= WARP_FLAG_FIGHT; //TODO: PLACE THIS AT MAX HEIGHT NODE
    //achieved_flags |= WARP_FLAG_INACCESSIBLE;
    //achieved_flags |= WARP_FLAG_ONEWAY;


    // TODO attach all one way and inaccessible nodes s.t.
        //{x,y},{a,b} INACCESSIBLE -> futher exits 
        //{x,y},{a,NULL} -> further exits
        //{x,NULL},{a,b} -> closer exit
    

    // TODO select pairs of loadzones s.t. no critical nodes are violated

    // TODO loop final exits on self

    //close off open tree exits (logic for attaching to nodes. also fine if node attaches to self (deadend)

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




