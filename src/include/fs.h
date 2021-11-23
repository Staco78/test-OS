#pragma once

#include "types.h"
#include "asm.h"
#include "terminal.h"

void readDisk(uint8 head, uint8 startSector, uint8 nbSector, uint16 cyl, uint8 *buff);
void writeDisk(uint8 head, uint8 startSector, uint8 nbSector, uint32 cyl, uint8 *buff);