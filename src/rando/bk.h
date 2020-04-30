#ifndef _BK_H
#define _BK_H

#include <stdint.h>
#include <libundermine.h>
#include "simptypes.h"

typedef struct{
    union{
        struct{
            uint16_t    a   : 1;
            uint16_t    b   : 1;
            uint16_t    z   : 1;
            uint16_t    s   : 1;
            uint16_t    du  : 1;
            uint16_t    dd  : 1;
            uint16_t    dl  : 1;
            uint16_t    dr  : 1;
            uint16_t        : 2;
            uint16_t    l   : 1;
            uint16_t    r   : 1;
            uint16_t    cu  : 1;
            uint16_t    cd  : 1;
            uint16_t    cl  : 1;
            uint16_t    cr  : 1;
        };
        uint16_t pad;
    };
    int8_t x;
    int8_t y;
} bk_controller_t;

typedef enum game_mode{
    BK_GAME_MODE_NORMAL = 0x03,
    BK_GAME_MODE_PAUSED = 0x04,
    BK_GAME_MODE_RECORDED_FILE = 0x06,
    BK_GAME_MODE_DEMO = 0x07,
    BK_GAME_MODE_BOTTLES_BONUS = 0x08,
    BK_GAME_MODE_BANJO_AND_KAZOOIE = 0x09,
    BK_GAME_MODE_SNS_PICTURE = 0x0a,
} bk_game_mode_t;

typedef enum level{
    BK_LEVEL_MM     = 0x01,
    BK_LEVEL_TTC    = 0x02,
    BK_LEVEL_CC     = 0x03,
    BK_LEVEL_BGS    = 0x04,
    BK_LEVEL_FP     = 0x05,
    BK_LEVEL_LAIR   = 0x06,
    BK_LEVEL_GV     = 0x07,
    BK_LEVEL_CCW    = 0x08,
    BK_LEVEL_RBB    = 0x09,
    BK_LEVEL_MMM    = 0x0a,
    BK_LEVEL_SM     = 0x0b,
    BK_LEVEL_FIGHT  = 0x0c,
    BK_LEVEL_CUTSCENE = 0x0d,
} bk_level_t;

typedef enum moves{
    bk_moves_beakBarge = 0,
    bk_moves_beakBomb = 1,
    bk_moves_beakBuster = 2,
    bk_moves_cameraControls = 3,
    bk_moves_bearPunch = 4,
    bk_moves_climbTrees = 5,
    bk_moves_eggs = 6,
    bk_moves_featheryFlap = 7,
    bk_moves_flapFlip = 8,
    bk_moves_flying = 9,
    bk_moves_holdAToJumpHigher = 10,
    bk_moves_ratATatRap = 11,
    bk_moves_roll = 12,
    bk_moves_shockSpringJump = 13,
    bk_moves_wadingBoots = 14,
    bk_moves_dive = 15,
    bk_moves_talonTrot = 16,
    bk_moves_turboTalonTrainers = 17,
    bk_moves_wonderwing = 18,
    bk_moves_firstNoteDoorMolehill = 19,
} bk_moves_t;

typedef enum transformations {
    BK_TRANS_BANJO = 0x01,
    BK_TRANS_TERMITE = 0X02,
    BK_TRANS_PUMPKIN = 0X03,
    BK_TRANS_WALRUS = 0X04,
    BK_TRANS_CROC = 0X05,
    BK_TRANS_BEE = 0X06,
    BK_TRANS_WISHYWASHY = 0X07,
} bk_transformation_t;

typedef enum bK_item{
    BK_ITEM_SKULL_HOURGLASS        = 0x1,
    BK_ITEM_PROPELLOR_TIMER        = 0x3,
    BK_ITEM_CHRISTMAS_TREE_TIMER   = 0x5, //crashes game
    BK_ITEM_HOURGLASS              = 0x6,
    BK_ITEM_MUSIC_NOTE             = 0xC,
    BK_ITEM_EGG                    = 0xD,
    BK_ITEM_JIGGY_LEVEL            = 0xE,
    BK_ITEM_RED_FEATHER            = 0xF,
    BK_ITEM_GOLD_FEATHER           = 0x10,
    BK_ITEM_JINJO                  = 0x12,
    BK_ITEM_EMPTY_HONEYCOMB        = 0x13,
    BK_ITEM_HEALTH                 = 0x14,
    BK_ITEM_HEALTH_MAX             = 0x15,
    BK_ITEM_LIFE                   = 0x16,
    BK_ITEM_AIR                    = 0x17,
    BK_ITEM_BLUBBERS_GOLD          = 0x18,
    BK_ITEM_CHIMPYS_ORANGE         = 0x19,
    BK_ITEM_VILE_SCORE_PLAYER      = 0x1A,
    BK_ITEM_VILE_SCORE_VILE        = 0x1B,
    BK_ITEM_MUMBO_TOKEN_UNUSED     = 0x1C,
    BK_ITEM_GRUMBLIE               = 0x1D,
    BK_ITEM_YUMBLIE                = 0x1E,
    BK_ITEM_PRESENT_GREEN          = 0x1F,
    BK_ITEM_PRESENT_BLUE           = 0x20,
    BK_ITEM_PRESENT_RED            = 0x21,
    BK_ITEM_CATEPILLAR             = 0x22,
    BK_ITEM_ACORN                  = 0x23,
    BK_ITEM_TWINKLIE               = 0x24,
    BK_ITEM_MUMBO_TOKEN_TOTAL      = 0x25,
    BK_ITEM_JIGGY_TOTAL            = 0x26,
    BK_ITEM_JOKER_CARD             = 0x27,
}bk_item_t;

typedef enum bk_global_flag{
    BK_G_FLAG_DOUBLE_HEALTH          = 0xB9,

    BK_G_FLAG_DOUBLE_EGGS            = 0xBE,
    BK_G_FLAG_DOUBLE_RED_FEATHERS    = 0xBF,
    BK_G_FLAG_DOUBLE_GOLD_FEATHERS   = 0xC0,
} bk_global_flag_t;

#define bk_extern       extern __attribute__ ((section(".data")))

bk_extern   uint32_t        load_code_stage2_hook;
bk_extern   uint32_t        game_update_hook;
bk_extern   uint32_t        load_code_stage1_hook;
bk_extern   uint32_t        gfx_finish_hook;
bk_extern   int32_t         bk_vi_counter;
bk_extern   bk_controller_t bk_controller_raw;
bk_extern   uint8_t         bk_map;
bk_extern   uint8_t         bk_high_note_scores;

bk_extern   uint32_t        bk_void_to_map_logic;
bk_extern   uint32_t        bk_deathwarp_take_me_there_hook;
bk_extern   uint32_t        bk_map_exit_no_reset_set_hook;
bk_extern   void            bk_map_exit_no_reset_set    (uint32_t map_indx, uint32_t exit_indx, uint32_t transition_type);

bk_extern   uint32_t        bk_map_savestate_ptr_array;           
bk_extern   uint32_t        bk_map_savestate_clear_all_hook1;
bk_extern   void            bk_map_savestate_clear_all(void);
bk_extern   uint32_t        bk_map_savestate_clear_hook;
//bk_extern   void            bk_map_savestate_clear(u32 map_indx);
bk_extern   uint32_t        bk_map_savestate_save_hook;
bk_extern   void            bk_map_savestate_save(u32 map_indx);

bk_extern   void*           bk_defrag(void *);
bk_extern   void            bk_free(void *);
bk_extern   void*           bk_realloc(void* src, size_t size);
bk_extern   void*           bk_malloc(size_t size);

bk_extern   uint32_t        bk_map_savestate_clearAll_freeCall;
bk_extern   uint32_t        bk_map_savestate_defrag_defragCall;
bk_extern   uint32_t        bk_map_savestate_save_freeCall;
bk_extern   uint32_t        bk_map_savestate_save_mallocCall;
bk_extern   uint32_t        bk_map_savestate_save_reallocCall;
bk_extern   uint32_t        bk_map_savestate_saveActors_reallocCall;
bk_extern   uint32_t        bk_map_savestate_apply_freeCall;

bk_extern   uint32_t        bk_restore_health_hook;
bk_extern   void            bk_restore_health(void);
bk_extern   uint32_t        bk_map_reset_hook;

bk_extern   uint32_t        bk_skip_intro_cutscene_hook;
bk_extern   uint32_t        bk_skip_lair_cutscene_hook;

bk_extern   int         game_update                     (void);
bk_extern   void        load_code_stage2                (void);
bk_extern   void        load_code_stage1                (void);
bk_extern   void        bk_gfx_finish                   (Gfx **p_gfx_p);
bk_extern   void        bk_take_me_there                (uint32_t map, uint32_t exit, uint32_t warp_type);
bk_extern   void        bk_transformation_set           (uint32_t transform);
bk_extern   uint32_t    bk_idle_movement_index_get      (void);
bk_extern   void        bk_player_movement_state_set    (uint32_t movement_state_indx);
bk_extern   void        bk_player_model_update          (void);    


bk_extern   void        bk_moves_unlocked_set (uint32_t move_index, uint32_t boolean);
bk_extern   bool        bk_moves_unlocked_get (uint32_t move_index);
bk_extern   void        bk_moves_has_used_set (uint32_t move_index); //code clear method
bk_extern   bool        bk_moves_has_used_get (uint32_t move_index);

bk_extern   uint32_t    bk_save_file_index_get(void);

bk_extern   bool        bk_files_has_data(uint32_t file_index);

bk_extern   bk_game_mode_t  bk_game_mode_get (void);

bk_extern   uint32_t    bk_item_get_count   (uint32_t item_index);
bk_extern   uint32_t    bk_item_array;

bk_extern   bool        bk_global_flag_get  (uint32_t g_flag_indx);
bk_extern   void        bk_global_flag_set  (uint32_t g_flag_indx, bool val);

bk_extern   bool        bk_jiggy_flag_get   (uint32_t jiggy_index);
bk_extern   void        bk_jiggy_flag_set   (uint32_t jiggy_index, bool val);

bk_extern   bool        bk_mumbo_token_flag_get   (uint32_t mt_index);
bk_extern   void        bk_mumbo_token_flag_set   (uint32_t mt_index, bool val);

bk_extern   bool        bk_empty_honeycomb_flag_get   (uint32_t honeycomb_index);
bk_extern   void        bk_empty_honeycomb_flag_set   (uint32_t honeycomb_index, bool val);

bk_extern   void        bk_printf   (uint32_t x, uint32_t y, char* str);

bk_extern   bk_level_t  bk_level_get(void);
bk_extern   uint32_t    bk_level_high_note_score_get(bk_level_t level);

bk_extern   void        bk_item_overlays_instaupdate_values(void);

bk_extern   uint32_t    bk_level_flags_get_n(uint32_t indx, uint32_t n);
bk_extern   void        bk_level_flags_set_n(uint32_t index, uint32_t val, uint32_t n);

bk_extern   uint32_t    bk_item_blubber_gold_reset_hook;
bk_extern   uint32_t    bk_item_conga_orange_reset_hook;
bk_extern   uint32_t    bk_item_nabnut_acorn_reset_hook;


bk_extern   uint32_t    bk_item_present_g_reset_hook;
bk_extern   uint32_t    bk_item_present_b_reset_hook;
bk_extern   uint32_t    bk_item_present_r_reset_hook;

bk_extern   uint32_t    bk_item_catepillar_reset_hook;
#endif
