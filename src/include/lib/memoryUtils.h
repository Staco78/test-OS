#pragma once
#include "types.h"

static void memcpy(void *destination, const void *source, uint32 size)
{
    for (uint32 i = 0; i < size; i++)
        ((uint8 *)destination)[i] = ((uint8 *)source)[i];
}

static void memset(void *ptr, uint8 value, uint32 size)
{
    for (uint32 i = 0; i < size; i++)
        ((uint8 *)ptr)[i] = value;
}