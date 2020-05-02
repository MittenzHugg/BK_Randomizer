#include <startup.h>
#include <stdlib.h>
#include <libundermine.h>
#include "rando.h"
#include "bk.h"
#include "commands.h"
#include "randoresource.h"
#include "start.h"
#include "sys.h"
#include "warps.h"

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

            if(bk_files_has_data(bk_save_file_index_get())){                
                rando_menu->selected_item = NULL;
            } else {
                rando_menu->selected_item = rando.seed_num;
            }
            rando.current_file = bk_save_file_index_get();
            menu_number_set(rando.seed_num,rando.seed[rando.current_file]);
            rando.current_seed = rando.seed[rando.current_file];
        }
        if(rando_menu->selected_item == NULL && !(bk_files_has_data(bk_save_file_index_get()))){
            rando_menu->selected_item = rando.seed_num;
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

static int rando_seed_update(event_handler_t *handler, menu_event_t event, void **event_data){
    uint32_t value = (uint32_t)*event_data;
    menu_number_set(handler->subscriber, value);
    rando.seed[rando.current_file] = value;
    return 1;
}

static int rando_mode_set(event_handler_t *handler, menu_event_t event, void **event_data){
    rando.mode[rando.current_file] = (rando_mode_t) handler->callback_data;
    menu_item_t *button = handler->subscriber;
    if( rando.mode[rando.current_file] == (rando_mode_t) handler->callback_data){
        button->color = COLOR_GREEN;
    }
    else {
        button->color = DEFAULT_COLOR;
    }
    return 1;
}

static int rando_mode_update(event_handler_t *handler, menu_event_t event, void **event_data){
    menu_item_t *button = handler->subscriber;
    rando_mode_t button_mode = (rando_mode_t) handler->callback_data;
    if( rando.mode[rando.current_file] == (rando_mode_t) handler->callback_data){
        button->color = COLOR_GREEN;
    }
    else {
        button->color = DEFAULT_COLOR;
    }
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

    menu_t *main_menu = &rando.main_menu;
    menu_t *pause_menu = &rando.pause_menu;
    menu_ctx_init(main_menu, NULL);
    menu_init(main_menu, 20, 30);

    menu_label_add(main_menu, 0, 3, "Seed:");
    rando.seed_num = menu_number_input_add(main_menu, 8, 3, 16, 8);
    menu_item_register_event(rando.seed_num,  MENU_EVENT_NUMBER, rando_seed_update, NULL);
    menu_number_set(rando.seed_num, 0xDEADBEEF);

    menu_label_add(main_menu, 0, 4, "Length:");
    menu_item_t* short_button = menu_button_add(main_menu, 8, 4, "Short",rando_mode_set, RANDO_MODE_SHORT);
    menu_item_register_event(short_button, MENU_EVENT_UPDATE,rando_mode_update, RANDO_MODE_SHORT);

    rando.cwd_name = (char *) malloc(40);
    menu_label_add(main_menu, 0, 5,rando.cwd_name);

    if(mkdir("bk_rando", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)){
        rando.cwd_name = "could not make dir \"\\bk_rando\\\"";
    }
    chdir("bk_rando");
        

    menu_item_t* normal_button = menu_button_add(main_menu, 14, 4, "Normal",rando_mode_set, RANDO_MODE_NORMAL);
    normal_button->color = COLOR_GREEN;
    menu_item_register_event(normal_button, MENU_EVENT_UPDATE,rando_mode_update, RANDO_MODE_NORMAL);

    menu_item_t* long_button = menu_button_add(main_menu, 21, 4, "Long",rando_mode_set, RANDO_MODE_LONG); 
    menu_item_register_event(long_button, MENU_EVENT_UPDATE,rando_mode_update, RANDO_MODE_LONG);

    menu_ctx_init(pause_menu, NULL);
    menu_init(pause_menu, 20, 30);
    menu_label_add(pause_menu, 0, 0, "Seed:");
    menu_item_t* pause_seed_number = menu_number_input_add(pause_menu, 6, 0, 10, 6);
    

    rando.menu_active = 0;
    rando.current_file = 0;
    rando.ready = 1;

    //TODO load file data from ED
    const char *s_eof = "unexpected end of file";
    const char *s_memory = "out of memory";
    const char *err_str = NULL;
    int f = open("./seeddata.bk_rand", O_RDONLY);
    if(f== -1) rando.cwd_name = "ERROR: OPENING seeddata.bk_rand";
    else{
        if (f != -1){
            struct stat st;
            fstat(f, &st);
            int n_seed = 3*sizeof(uint32_t);
            //read seeds
            if(read(f, rando.seed, n_seed) != n_seed){
                err_str = s_eof;
                goto f_err;
            }

            int n_length = 3*sizeof(rando_mode_t);
            //read length
            if(read(f, rando.mode, n_length) != n_length){
                err_str = s_eof;
                goto f_err;
            }
    f_err:
            sys_io_mode(SYS_IO_PIO);
            if (errno != 0)
                err_str = strerror(errno);
            else
                err_str = strerror(errno);
        }
    }
    if (f != -1)
        close(f);
    else{
        for(int i=0; i<3; i++){
            rando.seed[i]=0;
            rando.mode[i]=RANDO_MODE_NORMAL;
            menu_number_set(rando.seed_num, rando.seed[rando.current_file]);
        }
    }
    if (err_str)
        rando.cwd_name = err_str;
    
    rando.current_file = 0;
    rando.current_seed = rando.seed[rando.current_file];
    menu_number_set(pause_seed_number, rando.current_seed);

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

void start_new_wrapping(uint map,  uint exit, uint transition){
    //Save seed to ED file
    rando.cwd_name = getcwd(rando.cwd_name,40);
    int f = creat("./seeddata.bk_rand", O_WRONLY);
    if(f != -1){
        int n_seed = 3*sizeof(uint32_t);
        write(f,rando.seed, n_seed);
        
        int n_length = 3*sizeof(rando_mode_t);
        write(f, rando.mode, n_length);
        
        close(f);
    }
    else{
        rando.cwd_name = strerror(errno);
    }

    warp_generate_mapping();

    bk_take_me_there(map, exit, transition);
    return;
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

    //always skippable intro cutscene
    bk_skip_intro_cutscene_hook = 0x00000000;//li v0 0x01
    osInvalICache((void*)&bk_skip_intro_cutscene_hook, 4);
    
    //always skippable lair cutscene
    bk_skip_lair_cutscene_hook = 0x00000000;
    osInvalICache((void*)&bk_skip_lair_cutscene_hook, 4);
    
    // TODO always skip gameover cutscene
    
    rando_warps_init();

    //start new file hook
    uint32_t* start_file_lower_hook = 0x802c5104;
    uint32_t* start_file_upper_hook = 0x802c50f8;
    uint32_t warp_gen_p = (uint32_t)&start_new_wrapping; 
    uint32_t warp_gen_upper = (warp_gen_p >> 16) & 0xFFFF;
    uint32_t warp_gen_lower = warp_gen_p & 0xFFFF;
    if (warp_gen_lower>0x7FFF){
        ++warp_gen_upper;
    }
    *start_file_lower_hook &= 0xFFFF0000;
    *start_file_lower_hook |= warp_gen_lower;
    *start_file_upper_hook &= 0xFFFF0000;
    *start_file_upper_hook |= warp_gen_upper;
    osInvalICache((void*)&start_file_lower_hook, 4);
    osInvalICache((void*)&start_file_upper_hook, 4);


    rando_savestates_init();

    
}

void rando_load_stage1(void){
    load_code_stage1();

    uint32_t rando_load_stage2_p = (uint32_t)&rando_load_stage2;
    rando_load_stage2_p = ((rando_load_stage2_p & 0xFFFFFF) >> 2) | 0xC000000;
    load_code_stage2_hook = rando_load_stage2_p;
    osInvalICache((void*)&load_code_stage2_hook, 4);
}