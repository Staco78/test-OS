#pragma once

#include "types.h"
#include "asm.h"
#include "terminal.h"

void readDisk(uint32 address, uint16 nbSector, uint8 *buff);
void writeDisk(uint32 address, uint16 nbSector, uint8 *buff);