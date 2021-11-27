#pragma once
#include "types.h"
#include "terminal.h"
#include "panic.h"

void memory_init();
void *kmalloc();
void kfree(void *address);