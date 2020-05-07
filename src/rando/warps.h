#ifndef _WARPS_H
#define _WARPS_H
#include <stdlib.h>
#include "simptypes.h"

#define BIT(x)  (0x01 << x)

typedef enum warp_flags_open{
    WF_OPEN_MM      = 0,
    WF_OPEN_TTC     = 1,
    WF_OPEN_CC      = 2,
    WF_OPEN_BGS     = 3,
    WF_OPEN_FP      = 4,
    WF_OPEN_GV      = 5,
    WF_OPEN_MMM     = 6,
    WF_OPEN_RBB     = 7,
    WF_OPEN_CCW     = 8,
    WF_OPEN_SPRING  = 9,
    WF_OPEN_SUMMER  = 10,
    WF_OPEN_AUTUMN  = 11,
    WF_OPEN_WINTER  = 12,
    WF_OPEN_DOG     = 13,
} warp_flags_open_e;

typedef enum warp_flags_note{
    WF_50_NOTES     = 0,
    WF_180_NOTES    = 1,
    WF_260_NOTES    = 2,
    WF_350_NOTES    = 3,
    WF_450_NOTES    = 4,
    WF_640_NOTES    = 5,
    WF_765_NOTES    = 6,
    WF_810_NOTES    = 7,
}warp_flags_note_e;

typedef enum warp_flags_move{
    WF_TALON_TROT   = 0,
    WF_BEAK_BUST    = 1,
    WF_EGGS         = 2,
    WF_FLIGHT       = 3,
    WF_SHOCK_SPRING = 4,
    WF_WADING_BOOTS = 5,
    WF_BEAK_BOMB    = 6,
    WF_TURBO_TALON  = 7,
}warp_flags_move_e;

#define WF_TRANS_ANY 0x1F
typedef enum warp_flag_tranformations{
    WF_TRANS_TERMITE   = 0X00,
    WF_TRANS_PUMPKIN   = 0X01,
    WF_TRANS_WALRUS    = 0X02,
    WF_TRANS_CROC      = 0X03,
    WF_TRANS_BEE       = 0X04,
} warp_flag_tranformations_e;

typedef enum warp_flag_misc{
    WF_INACCESSIBLE     = 0x00,
    WF_ONEWAY_OUT       = 0X01,
    WF_ONEWAY_IN        = 0X02,
    WF_GV_WATER         = 0X03,
    WF_WATER_LVL2    = 0X04,
    WF_WATER_LVL3    = 0X05,
    WF_FIGHT         = 0x06,
} warp_flag_misc_e;

#define WF_HARD_NONE 0,0,0
#define WF_SOFT_NONE 0,0,0

typedef struct warp_hard_flags_struct{
    u16 level_open;
    u8  note_door;
    u8  moves;
}warp_hard_flags_t;

typedef struct warp_soft_flags_struct{
    u8  moves;
    u8  transform;
    u8  misc;
}warp_soft_flags_t;

//50, 180, 260, 350, 450, 640, 765, 810
typedef struct exit{
    u8 map;
    u8 exit;
} exit_t;

typedef struct exitMap{
    exit_t old;
    exit_t new;
} exitMap_t;

typedef struct warp{
    exit_t  me;
    exit_t  ret;
    warp_hard_flags_t     hard_flags; //all MUST be satisfied
    warp_soft_flags_t     soft_flags; //any MUST be satisfied
} warp_t;

typedef struct warp_pool{
    u32 len;
    warp_t* pool[256];
}warp_pool_t;

#define NODEPOOL_A_INIT_SIZE 153
#define NODEPOOL_U_INIT_SIZE 90
#define TREE_A_INIT_SIZE 2
#define TREE_U_INIT_SIZE 4

extern warp_t     nodePool_available_init[NODEPOOL_A_INIT_SIZE];
extern warp_t     nodePool_unavailable_init[NODEPOOL_U_INIT_SIZE];
extern warp_t     tree_available_init[TREE_A_INIT_SIZE];
extern warp_t     tree_unavailable_init[TREE_U_INIT_SIZE];

extern const u16 hash_table_len; //about 1.3x exit count
extern exitMap_t exitLUT[337];

//extern const warp_t    vanilla_pairs[];

exitMap_t* exitLUT_get(exit_t* old);
void wm_generate_mapping(u32 seed);

#endif