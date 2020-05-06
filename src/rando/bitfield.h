#ifndef _BITFIELD_H_
#define _BITFIELD_H_
#include "simptypes.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>


typedef struct bitfield bitfield_t;

struct bitfield{
    u8* ptr;
    u32 max_i;
    u32 byte_count;
};

bitfield_t* bitfield_new(u32 max_i);
void bitfield_free(bitfield_t* field);
void bitfield_clear(bitfield_t* field);
bool bitfield_get(bitfield_t* field, u32 indx);
u32  bitfield_get_count(bitfield_t* field);
void bitfield_set(bitfield_t* field, u32 indx, bool value);
bool bitfield_equal(bitfield_t* A, bitfield_t* B);
bool bitfield_overlap(bitfield_t* A, bitfield_t* B);
#endif