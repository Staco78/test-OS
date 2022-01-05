#pragma once

#include "types.h"

void readDisk(uint32 startSector, uint16 nbSector, void *buff);
void writeDisk(uint32 startSector, uint16 nbSector, void *buff);
void fs_start();