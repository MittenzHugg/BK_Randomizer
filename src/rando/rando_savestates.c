#include <stdint.h>
#include <string.h>
#include "simptypes.h"
#include "bk.h"

static const save_ratio = 4;

static uint32_t level_jinjos[0x0E];

void rando_restore_items(void){
    bk_restore_health(); //Hooked Function DO NOT REMOVE

    bk_level_t currLevel = bk_level_get();

    (&bk_item_array)[BK_ITEM_JINJO] = level_jinjos[currLevel];
    if(currLevel != BK_LEVEL_LAIR){
        (&bk_item_array)[BK_ITEM_MUSIC_NOTE] = bk_level_high_note_score_get(currLevel);
    }
    //ToDo: call insta-update print item totals
    bk_item_overlays_instaupdate_values();
}

void rando_save_items(void){
    bk_level_t currLevel = bk_level_get();
    level_jinjos[currLevel] = bk_item_get_count(BK_ITEM_JINJO);

}

void rando_ss_free(void * ptr){
    if(ptr >=  0x8002D500 && ptr < 0x8023DA20){
        bk_free(ptr);
    } else {
        free(ptr);
    }
}

void* rando_ss_defrag(void * ptr){
    if(ptr >=  0x8002D500 && ptr < 0x8023DA20){
        bk_defrag(ptr); //TODO: don't actually know what this function does...
    } else {
        return ptr;
    }
}


static u32 savestate_map = 0; //required for map saving
void* rando_ss_realloc(void* src, size_t size){
    if(!(savestate_map % save_ratio)){
        return realloc(src, size);
    }else{
        return bk_realloc(src, size);
    }
}

void* rando_ss_malloc(size_t size){
    if(!(savestate_map % save_ratio)){
        return malloc(size);
    }else{
        return bk_malloc(size);
    }
}

void rando_savestates_save(u32 map_indx){
    savestate_map = map_indx;

    //create new struct
    bk_map_savestate_save(map_indx);
    rando_save_items();
}

void rando_savestates_init(void){
    //prevent map savestate clear
    bk_map_savestate_clear_all_hook1 = 0x00000000;//nop
    osInvalICache((void*)&bk_map_savestate_clear_all_hook1, 4);
    bk_map_savestate_clear_hook = 0x00000000;//nop
    osInvalICache((void*)&bk_map_savestate_clear_hook, 4);

    /* FUNCTIONS FOR SAVESTATE MEMORY MANAGEMENT*/
    //savestate saving
    uint32_t ss_save_p = (uint32_t)&rando_savestates_save;
    ss_save_p = ((ss_save_p & 0xFFFFFF) >> 2 ) | 0xC000000;
    bk_map_savestate_save_hook = ss_save_p;
    osInvalICache((void*)&bk_map_savestate_save_hook, 4);

    //bk_free calls
    uint32_t free_p = (uint32_t)&rando_ss_free;
    free_p = ((free_p & 0xFFFFFF) >> 2 ) | 0xC000000;
    bk_map_savestate_clearAll_freeCall = free_p;
    bk_map_savestate_save_freeCall = free_p;
    bk_map_savestate_apply_freeCall = free_p;
    osInvalICache((void*)&bk_map_savestate_clearAll_freeCall, 4);
    osInvalICache((void*)&bk_map_savestate_save_freeCall, 4);
    osInvalICache((void*)&bk_map_savestate_apply_freeCall, 4);

    uint32_t defrag_p = (uint32_t)&rando_ss_defrag;
    defrag_p = ((defrag_p & 0xFFFFFF) >> 2 ) | 0xC000000;
    bk_map_savestate_defrag_defragCall = defrag_p;
    osInvalICache((void*)&bk_map_savestate_defrag_defragCall, 4);

    //bk_malloc calls
    uint32_t malloc_p = (uint32_t)&rando_ss_malloc;
    malloc_p = ((malloc_p & 0xFFFFFF) >> 2 ) | 0xC000000;
    bk_map_savestate_save_mallocCall = malloc_p;
    osInvalICache((void*)&bk_map_savestate_save_mallocCall, 4);

    //bk_realloc calls
    uint32_t realloc_p = (uint32_t)&rando_ss_realloc;
    realloc_p = ((realloc_p & 0xFFFFFF) >> 2 ) | 0xC000000;
    bk_map_savestate_save_reallocCall = realloc_p;
    bk_map_savestate_saveActors_reallocCall = realloc_p;
    osInvalICache((void*)&bk_map_savestate_save_reallocCall, 4);
    osInvalICache((void*)&bk_map_savestate_saveActors_reallocCall, 4);

    //item reload on level enter
    uint32_t reload_p = (uint32_t)&rando_restore_items;
    reload_p = ((reload_p & 0xFFFFFF) >> 2 ) | 0xC000000;
    bk_restore_health_hook = reload_p;
    osInvalICache((void*)&bk_restore_health_hook, 4);
}