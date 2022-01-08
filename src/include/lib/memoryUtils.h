#pragma once
#include "types.h"

void memcpy(void *destination, const void *source, uint32 size)
{
    for (uint32 i = 0; i < size; i++)
        ((uint8 *)destination)[i] = ((uint8 *)source)[i];
}