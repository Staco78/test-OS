#pragma once

#include "types.h"
#include "asm.h"
#include "terminal.h"
#include "panic.h"
#include "memory.h"

void readDisk(uint32 startSector, uint16 nbSector, void *buff);
void writeDisk(uint32 startSector, uint16 nbSector, void *buff);
void fs_start();