#pragma once
#include "types.h"

void memory_init();
void *kmalloc();
void kfree(void *address);
void *sized_kmalloc(uint32 size);
void sized_free(uint32 size);