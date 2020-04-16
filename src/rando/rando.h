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
    uint32_t    seed[4];
    rando_mode_t mode[4];
    menu_item_t* seed_num;
} rando_ctxt_t;



extern rando_ctxt_t rando;

void warp_interceptor(uint32_t map_indx, uint32_t exit_indx, uint32_t transition_type);
void warp_clear_saveState_interceptor(void);
void warp_map_flag_clear_interceptor(uint32_t map_indx);
#endif
