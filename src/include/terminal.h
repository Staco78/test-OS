#pragma once
#include "types.h"

void terminalInit();
void clear();
void printChar(uint8 c);
void printInt(uint32 n);
void printHex(uint32 n);
void print8bitHex(uint8 n);
void printCustomHex(uint32 *data);
void printBin(uint8 n);
void print(const char *str);
void printLn();