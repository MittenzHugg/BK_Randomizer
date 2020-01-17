#include <stdlib.h>
#include "input.h"
#include "commands.h"
#include "resource.h"
#include "bk.h"

#define INPUT_REPEAT 8

uint32_t button_colors[16] = {
    0xFFF000FF,
    0xFFF000FF,
    0xFFF000FF,
    0xFFF000FF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0,
    0,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFF823CFF,
    0xFFFFFFFF,
    0x64FF78FF,
    0x64C8FFFF,
};

 enum bind_state {
    BIND_STATE_NONE,
    BIND_STATE_ACTION,
    BIND_STATE_BEGIN,
    BIND_STATE_LISTENING,
};

struct bind_item_data {
    int             cmd;
    enum bind_state state;
};

static int      button_time[16];
static uint16_t pad_pressed_raw;
static uint16_t pad_pressed;
static uint16_t pad_released;
static uint16_t pad;
static uint16_t reserved;
static int8_t   stick_x;
static int8_t   stick_y;
static int      bind_component_state[KB_CMD_MAX];
static int      bind_time[KB_CMD_MAX];
static _Bool    bind_pressed_raw[KB_CMD_MAX];
static _Bool    bind_pressed[KB_CMD_MAX];
static _Bool    input_enabled = 1;

static int bind_get_component(uint16_t bind, int index){
    return (bind >> (4 * index)) & 0xF;
}

static uint16_t bind_get_bitmask(uint16_t bind){
    uint16_t p = 0;
    for(int i = 0;i < 4;i++){
        int c = bind_get_component(bind, i);
        if(c == BIND_END){
            break;
        }
        p |= 1 << c;
    }
    return p;
}

uint16_t make_bind(int len, ...){
    va_list list;
    va_start(list, len);
    uint16_t ret = 0;
    uint16_t val = 0;
    for(int i = 0;i < len;i++){
        val = va_arg(list, int);
        for(int j = 0;j < 16;j++){
            if(val & (1 << j)){
                ret |= j << (i * 4);
            }
        }
    }
    va_end(list);
    if(len < 4){
        ret |= 6 << (len * 4);
    }
    return ret;
}

void input_update(void){
    uint16_t z_pad = bk_controller_raw.pad;
    stick_x = bk_controller_raw.x;
    stick_y = bk_controller_raw.y;
    pad_pressed_raw = (pad ^ z_pad) & z_pad;
    pad_released = (pad ^ z_pad) & ~z_pad;
    pad = z_pad;
    pad_pressed = 0;
    for(int i = 0;i < 16;i++){
        uint16_t p = 1 << i;
        if(pad & p){
            button_time[i]++;
        }else{
            button_time[i] = 0;
        }
        if((pad_pressed_raw & p) || button_time[i] >= INPUT_REPEAT){
            pad_pressed |= p;
        }
    }
    uint16_t bind_pad[KB_CMD_MAX];
    _Bool bind_state[KB_CMD_MAX];
    for(int i = 0;i < KB_CMD_MAX;i++){
        uint16_t *b =  &kb_binds[i];
        bind_pad[i] = bind_get_bitmask(*b);
        int *cs = &bind_component_state[i];
        int j = 0;
        uint16_t c = BIND_END;
        if(((reserved & bind_pad[i]) && i != 0 && i != KB_CMD_RETURN) || !input_enabled){
            *cs = 0;
        }else{
            int css = *cs;
            for(j = 0;j < 4;j++){
                c = bind_get_component(*b, j);
                if(c == BIND_END){
                    break;
                }
                uint8_t csm = 1 << j;
                if(*cs & csm){
                    if(pad & (1 << c)){
                        continue;
                    }
                    else{
                        if(*cs & ~((1 << (j + 1)) - 1)){
                            *cs = 0;
                        }else{
                            *cs &= ~csm;
                        }
                        break;
                    }
                }
                if((pad_released & (1 << c)) || (css != 0 && (pad_pressed_raw & ~bind_pad[i]))){
                    *cs = 0;
                    break;
                }else if(pad_pressed_raw & (1 << c)){
                    *cs |= csm;
                }else{
                    break;
                }
            }
        }
        bind_state[i] = (*cs && (j == 4 || c == BIND_END));
    }
    for(int i = 0;i < KB_CMD_MAX;i++){
        uint16_t pi = bind_pad[i];
        for(int j = 0;bind_state[i] && j < KB_CMD_MAX;j++){
            if(!bind_state[j]){
                continue;
            }
            uint16_t pj = bind_pad[j];
            if(pi != pj && (pi & pj) == pi){
                bind_component_state[i] = 0;
                bind_state[i] = 0;
            }
        }
        bind_pressed_raw[i] = (bind_time[i] == 0 && bind_state[i]);
        if(!bind_state[i])
            bind_time[i] = 0;
        else{
            bind_time[i]++;
        }
        bind_pressed[i] = (bind_pressed_raw[i] || bind_time[i] >= INPUT_REPEAT);
    }
}

_Bool input_bind_held(int index){
    return bind_time[index] > 0;
}

_Bool input_bind_pressed(int index){
    return bind_pressed[index];
}

_Bool input_bind_pressed_raw(int index){
    return bind_pressed_raw[index];
}

void reserve_buttons(uint16_t button_bitmask){
    reserved |= button_bitmask;
}

void free_buttons(uint16_t button_bitmask){
    reserved ^= button_bitmask;
}

uint16_t input_pressed(void){
    if(!input_enabled){
        return 0;
    }
    return pad_pressed;
}

uint16_t input_pressed_raw(void){
    return pad;
}

int8_t input_x(void){
    return stick_x;
}

int8_t input_y(void){
    return stick_y;
}