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
    uint32_t    seed;
    menu_item_t* seed_num;
} rando_ctxt_t;



extern rando_ctxt_t rando;

menu_t *create_transformations_menu(void);
menu_t *create_warps_menu(void);
menu_t *create_watches_menu (void);
menu_t *create_moves_menu(void);
#endif
