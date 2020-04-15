#include <startup.h>
#include <stdlib.h>
#include <libundermine.h>
#include "rando.h"
#include "bk.h"
#include "commands.h"
#include "randoresource.h"
#include "start.h"

__attribute__((section(".data")))
rando_ctxt_t rando = {
    .ready = 0,
};

void rando_main(void){
    gfx_begin();
    input_update();

    void *event_data = NULL;
    rando.menu_active = (bk_map == 0x91); //activate menu when on select screen 0x91
    if(rando.menu_active){

        menu_t *rando_menu = &rando.main_menu;
        if(rando.current_file != bk_save_file_index_get()){
            // TODO save seed to ED file
            // TODO load seed from ED file
            if(bk_files_has_data(bk_save_file_index_get())){                
                rando_menu->selected_item = NULL;
            } else {
                rando_menu->selected_item = rando.seed_num;
            }
            rando.current_file = bk_save_file_index_get();
        }
        
        if(!bk_files_has_data(rando.current_file)){
            uint16_t pressed = input_pressed();
            if(pressed & BUTTON_D_DOWN){
                menu_trigger_event(rando_menu, MENU_EVENT_NAVIGATE, (void*)MENU_NAV_DOWN);
            }
            if(pressed & BUTTON_D_UP){
                menu_trigger_event(rando_menu, MENU_EVENT_NAVIGATE, (void*)MENU_NAV_UP);
            }
            if(pressed & BUTTON_D_LEFT){
                menu_trigger_event(rando_menu, MENU_EVENT_NAVIGATE, (void*)MENU_NAV_LEFT);
            }
            if(pressed & BUTTON_D_RIGHT){
                menu_trigger_event(rando_menu, MENU_EVENT_NAVIGATE, (void*)MENU_NAV_RIGHT);
            }
            if(pressed & BUTTON_L){
                menu_trigger_event(rando_menu, MENU_EVENT_ACTIVATE, &event_data);
            
            }
            menu_trigger_event(rando_menu, MENU_EVENT_UPDATE, &event_data);
        }
        menu_trigger_event(rando_menu, MENU_EVENT_UPDATE, &event_data);
        menu_draw(rando_menu);
    }
    else if (bk_game_mode_get() == BK_GAME_MODE_PAUSED){
        menu_t *pause_menu = &rando.pause_menu;
        menu_draw(pause_menu);
    }
}

static int main_menu_on_activate(event_handler_t *handler, menu_event_t event, void **event_data){
    //rando.menu_active = 0;
    return 1;
}

void init(void){
    clear_bss();
    do_global_ctors();

    input_init(&bk_controller_raw.pad, &bk_controller_raw.x, &bk_controller_raw.y, rando_binds, RANDO_CMD_MAX);
    bind_override(RANDO_CMD_TOGGLE_MENU);
    bind_override(RANDO_CMD_RETURN);

    rando_resource_init();
    gfx_init(0x32800, resource_get(resource_handles[R_RANDO_FONT]), NULL);

    //list_init(&rando.watches, sizeof(watch_t));

    menu_t *main_menu = &rando.main_menu;
    menu_t *pause_menu = &rando.pause_menu;
    menu_ctx_init(main_menu, NULL);
    menu_init(main_menu, 20, 30);


    //main_menu->selected_item = menu_button_add(main_menu, 0, 0, "return", main_menu_on_activate, NULL);
    
    menu_label_add(main_menu, 0, 3, "Seed:");
    rando.seed_num = menu_number_input_add(main_menu, 8, 3, 16, 8);
    main_menu->selected_item = rando.seed_num;
    menu_number_set(rando.seed_num, 0xDEADBEEF);

    menu_label_add(main_menu, 0, 4, "Length:");
    menu_button_add(main_menu, 8, 4, "Short",main_menu_on_activate, NULL); // TODO set callback 
    menu_button_add(main_menu, 14, 4, "Normal",main_menu_on_activate, NULL); // TODO set callback 
    menu_button_add(main_menu, 21, 4, "Long",main_menu_on_activate, NULL); // TODO set callback 

    menu_ctx_init(pause_menu, NULL);
    menu_init(pause_menu, 20, 30);
    menu_label_add(pause_menu, 0, 0, "Seed:");
    menu_item_t* pause_seed_number = menu_number_input_add(pause_menu, 6, 0, 10, 6);
    menu_number_set(pause_seed_number, 0x00);


    rando.menu_active = 0;
    rando.current_file = 0;
    rando.ready = 1;
    rando_binds[RANDO_CMD_TOGGLE_MENU] = make_bind(2, BUTTON_R, BUTTON_L);
    rando_binds[RANDO_CMD_RETURN] = make_bind(2, BUTTON_R, BUTTON_D_LEFT);
}

int _main(void){
    init_gp();
    if(!rando.ready){
        init();
    }
    _entry_data.exit_func = game_update;
    int ret = (int)_rando_exit();
    rando_main();
    return ret;
}

void rando_gfx_append(Gfx **p_gfx_p){
    gfx_finish_set(p_gfx_p);
    gfx_finish();
    bk_gfx_finish(p_gfx_p);
}

void rando_load_stage2(void){
    load_code_stage2();

    uint32_t main_p = (uint32_t)&_rando_entry;
    main_p = ((main_p & 0xFFFFFF) >> 2) | 0xC000000;
    game_update_hook = main_p;

    uint32_t overlay_p = (uint32_t)&rando_gfx_append;
    overlay_p = ((overlay_p & 0xFFFFFF) >> 2 ) | 0xC000000;
    gfx_finish_hook = overlay_p;

    osInvalICache((void*)&game_update_hook, 4);
    osInvalICache((void*)&gfx_finish_hook, 4);
}

void rando_load_stage1(void){
    load_code_stage1();

    uint32_t rando_load_stage2_p = (uint32_t)&rando_load_stage2;
    rando_load_stage2_p = ((rando_load_stage2_p & 0xFFFFFF) >> 2) | 0xC000000;
    load_code_stage2_hook = rando_load_stage2_p;

    osInvalICache((void*)&load_code_stage2_hook, 4);
}