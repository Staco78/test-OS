#pragma once
#include "types.h"
#include "asm.h"

void terminalInit();
void clear();
void makeCaps(uint8 active);
void printChar(uint8 c);
void printInt(uint32 n);
void print(uint8 *str);