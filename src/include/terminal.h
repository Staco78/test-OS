#pragma once
#include "types.h"
#include "asm.h"

void terminalInit();
void clear();
void printChar(uint8 c);
void printInt(uint32 n);
void printHex(uint32 n);
void print8bitHex(uint8 n);
void printBin(uint8 n);
void print(uint8 *str);
void printLn();