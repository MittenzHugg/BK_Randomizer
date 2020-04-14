#include "randoresource.h"

static const char *resource_names[R_RANDO_END] = {
    "font",
    "buttons",
};

static resource_ctor resource_ctors[R_RANDO_END] = {
    grc_ctor,
    grc_ctor,
};

int resource_handles[R_RANDO_END] = { -1 };

void rando_resource_init(){
    for(int i = 0;i < R_RANDO_END;i++){
        resource_ent_t *res = resource_create(resource_names[i], resource_ctors[i], NULL);
        if(res){
            resource_handles[i] = res->resource_id;
        }
    }
}