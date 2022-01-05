#pragma once
#include "types.h"
#include "terminal.h"
#include "panic.h"

void memory_init();
void kfree(void *address);
void *kmalloc(uint32 size);

void printMemoryUsage();