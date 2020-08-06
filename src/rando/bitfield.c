#include <stddef.h>
#include "bitfield.h"
#include <string.h>

bitfield_t* bitfield_new(u32 max_i){
    bitfield_t* return_value = (bitfield_t *) malloc(sizeof(bitfield_t));
    u32 shift = max_i - 1;
    u32 byte = (shift) >> 3;
    if(shift % 8){
        byte++;
    }
    return_value->byte_count = byte;
    return_value->max_i = max_i;
    return_value->ptr = (u8*) calloc(sizeof(u8), byte);
    return return_value;
}

void bitfield_free(bitfield_t* field){
    free(field->ptr);
    free(field);
    return;
}

void bitfield_clear(bitfield_t* field){
    memset(field->ptr,0,sizeof(u8)*field->byte_count);
    return;
}

bool bitfield_get(bitfield_t* field, u32 indx){
    u32 shift = indx - 1;
    u32 byte = (shift) >> 3;
    shift = shift % 8;
    u8 byte_val = *(field->ptr + byte);
    return (byte_val >> shift) & 0x01; 
}

void bitfield_set(bitfield_t* field, u32 indx, bool value){
    u32 shift = indx - 1;
    u32 byte = (shift) >> 3;
    shift = shift % 8;
    u8* byte_val = field->ptr + byte;
    if(value){
        *byte_val |= 1 << shift;
    }
    else{
        *byte_val &= ~(1 << shift);
    }
    return;
}

u32 bitfield_get_count(bitfield_t* field){
    u32 cnt = 0;
    for(u32 i = 0; i<field->max_i; i++){
        if(bitfield_get(field, i+1)) 
            cnt++;
    }
    return cnt;
}

bool bitfield_equal(bitfield_t* A, bitfield_t* B){
    if(A->max_i != B->max_i)
        return false;

    //compare bitfield values
    return !memcmp(A->ptr,B->ptr,A->byte_count);
}

bool bitfield_overlap(bitfield_t* A, bitfield_t* B){
    if(A->max_i != B->max_i)
        return false;

    //compare bitfield values
    for(u32 i = 0; i < A->byte_count; i++){
        if(*(A->ptr + i) & *(B->ptr + i))
            return true;
    }
    return false;
}
