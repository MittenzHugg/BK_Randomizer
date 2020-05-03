#ifndef _WARPS_H
#define _WARPS_H
#include <stdlib.h>
#include "simptypes.h"

#define WARP_FLAG_NONE          0
#define WARP_FLAG_FIGHT         0x1 << 0x01
#define WARP_FLAG_TTC_OPEN      0x1 << 0x02
#define WARP_FLAG_CC_OPEN       0x1 << 0x04
#define WARP_FLAG_BGS_OPEN      0x1 << 0x05
#define WARP_FLAG_FP_OPEN       0x1 << 0x06
#define WARP_FLAG_MMM_OPEN      0x1 << 0x07
#define WARP_FLAG_RBB_OPEN      0x1 << 0x08
#define WARP_FLAG_CCW_OPEN      0x1 << 0x09
#define WARP_FLAG_DOG_OPEN      0x1 << 0x0A
#define WARP_FLAG_50_NOTES      0x1 << 0x0B
#define WARP_FLAG_180_NOTES     0x1 << 0x0C
#define WARP_FLAG_260_NOTES     0x1 << 0x0D
#define WARP_FLAG_350_NOTES     0x1 << 0x0E
#define WARP_FLAG_450_NOTES     0x1 << 0x0F
#define WARP_FLAG_640_NOTES     0x1 << 0x10
#define WARP_FLAG_765_NOTES     0x1 << 0x11
#define WARP_FLAG_810_NOTES     0x1 << 0x12
#define WARP_FLAG_TALON_TROT    0x1 << 0x13
#define WARP_FLAG_BEAK_BUST     0x1 << 0x14
#define WARP_FLAG_EGGS          0x1 << 0x14
#define WARP_FLAG_SHOCK_SPRING  0x1 << 0x15
#define WARP_FLAG_FLIGHT        0x1 << 0x16
#define WARP_FLAG_WADING_BOOTS  0x1 << 0x17
#define WARP_FLAG_BEAK_BOMB     0x1 << 0x18
#define WARP_FLAG_TURBO_TALON   0x1 << 0x19
#define WARP_FLAG_TERMITE       0x1 << 0x1A
#define WARP_FLAG_CROC          0x1 << 0x1B
#define WARP_FLAG_WALRUS        0x1 << 0x1C
#define WARP_FLAG_PUMPKIN       0x1 << 0x1D
#define WARP_FLAG_BEE           0x1 << 0x1E
#define WARP_FLAG_INACCESSIBLE  0x1 << 0x1F
#define WARP_FLAG_ONEWAY        0x1 << 0x20
#define WARP_FLAG_GV_WATER      0x1 << 0x21
#define WARP_FLAG_GV_OPEN       0x1 << 0x22
#define WARP_FLAG_WATER_LVL2    0x1 << 0x23
#define WARP_FLAG_WATER_LVL3    0x1 << 0x24
#define WARP_FLAG_SPRING_OPEN   0x1 << 0x25
#define WARP_FLAG_SUMMER_OPEN   0x1 << 0x26
#define WARP_FLAG_AUTUMN_OPEN   0x1 << 0x27
#define WARP_FLAG_WINTER_OPEN   0x1 << 0x28



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
    u64     hard_flags; //all MUST be satisfied
    u64     soft_flags; //any MUST be satisfied
} warp_t;

extern warp_t     nodePool_available_init[153];
extern warp_t     nodePool_unavailable_init[92];
extern warp_t     tree_available_init[2];
extern warp_t     tree_unavailable_init[4];

extern const u16 hash_table_len; //about 1.3x exit count
extern exitMap_t exitLUT[337];

//extern const warp_t    vanilla_pairs[];

exitMap_t* exitLUT_get(exit_t* old);
void wm_generate_mapping(u32 seed);

#endif