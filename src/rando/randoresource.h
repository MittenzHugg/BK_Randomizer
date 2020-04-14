/*
* resource.h
*
* definitions for loading graphic resources
*/

#ifndef _RANDORESOURCE_H
#define _RANDORESOURCE_H
#include <libundermine.h>

enum resource {
    R_RANDO_FONT,
    R_RANDO_BUTTON,
    R_RANDO_END
};

void            rando_resource_init    (void);

extern int resource_handles[R_RANDO_END];

#endif