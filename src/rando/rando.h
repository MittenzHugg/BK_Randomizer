#ifndef _RANDO_H
#define _RANDO_H

#include <list/list.h>
#include <libundermine.h>

typedef struct {
    _Bool       ready;
    menu_t      main_menu;
    int         menu_active;
    char       *tooltip;
    struct list watches;
} rando_ctxt_t;

extern rando_ctxt_t rando;

menu_t *create_transformations_menu(void);
menu_t *create_warps_menu(void);
menu_t *create_watches_menu (void);
menu_t *create_moves_menu(void);
#endif
