#ifndef _SCENES_H
#define _SCENES_H
#include <stdint.h>

typedef struct rando_scene_category       rando_scene_category_t;
typedef struct rando_entrance             rando_entrance_t;
typedef struct rando_scene                rando_scene_t;

struct rando_scene_category {
    char        *name;
    uint32_t    scene_cnt; //0 if subcategory
    uint32_t    subcategory_type; //0 if scene, 1 if category
    union {
        rando_scene_category_t    *categories;
        rando_scene_t             *scenes;
    };
};

struct rando_scene {
    char             *name;
    uint32_t           index;
    uint32_t           entrance_cnt;
    rando_entrance_t    *entrances;
};

struct rando_entrance {
    uint32_t     index;
    char       *name;
} ;

extern rando_scene_category_t    scene_categories[13];

#endif