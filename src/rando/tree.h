#ifndef _TREE_H_
#define _TREE_H
#include <stdlib.h>
#include "simptypes.h"
#include "map.h"
#include "bitfield.h"
#include "warps.h"

#define MOVES_MAX_INDEX 9
typedef enum moves{
    MOVE_TALON_TROT     = 0x01,
    MOVE_BEAK_BUST      = 0x02,
    MOVE_EGGS           = 0x03,
    MOVE_FLIGHT         = 0x04,
    MOVE_SHOCK_SPRING   = 0x05,
    MOVE_WADING_BOOTS   = 0x06,
    MOVE_BEAK_BOMB      = 0x07,
    MOVE_TURBO_TRAINERS = 0x08,
    MOVE_WONDERWING     = 0x09
} moves_e;

typedef enum tranformations{
    TRANS_BANJO = 0x01,
    TRANS_TERMITE = 0X02,
    TRANS_PUMPKIN = 0X03,
    TRANS_WALRUS = 0X04,
    TRANS_CROC = 0X05,
    TRANS_BEE = 0X06,
} transform_e;

typedef struct tree_node{
    exit_t id; //0 if not divided room
    u8 depth;
    u8 parent;
} tree_node_t;

typedef struct tree{
    u8 depth; 
    u16 note_count;
    u8 token_count;
    u8 token_spent;
    u8 jiggy_count;
    u8 jiggy_spent;
    bitfield_t* jiggies;
    bitfield_t* maps;
    bitfield_t* moves;
    bitfield_t* tranformations;
    warp_pool_t* available_exits;
    warp_pool_t* unavailable_exits;
    tree_node_t* nodes;
    u8 root;
    warp_hard_flags_t hard_f;
    warp_soft_flags_t soft_f;
} tree_t;

tree_t* tree_init(exit_t* root_info, warp_pool_t* avail_list, warp_pool_t* unaviail_list);
void tree_free(tree_t* this);
#endif