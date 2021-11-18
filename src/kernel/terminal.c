#include "terminal.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

static uint8 *memory_address;

uint8 cursorX = 0;
uint8 cursorY = 0;

void terminalInit()
{
    memory_address = (uint8 *)0xB8000;
}

void scrollLine()
{
    cursorY--;
    for (uint16 i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        memory_address[i * 2] = memory_address[(i + SCREEN_WIDTH) * 2];
        memory_address[i * 2 + 1] = memory_address[(i + SCREEN_WIDTH) * 2 + 1];
    }
}

void printChar(uint8 c)
{

    if (c == '\n')
    {
        cursorX = 0;
        cursorY++;
    }
    else
    {
        memory_address[(cursorY * SCREEN_WIDTH + cursorX) * 2] = c;
        memory_address[(cursorY * SCREEN_WIDTH + cursorX) * 2 + 1] = 0x02;
        cursorX++;
    }

    if (cursorX >= SCREEN_WIDTH)
    {
        cursorX = 0;
        cursorY++;
    }
    if (cursorY >= SCREEN_HEIGHT)
        scrollLine();

    uint16 pos = cursorY * SCREEN_WIDTH + cursorX;

    write_port(0x3D4, 0x0F);
    write_port(0x3D5, (uint8)(pos & 0xFF));
    write_port(0x3D4, 0x0E);
    write_port(0x3D5, (uint8)((pos >> 8) & 0xFF));
}

void printInt(uint32 n)
{
    uint8 i = 0;
    uint8 str[16];

    for (uint8 i = 0; i < 16; i++)
        str[i] = 0;

    do
    {
        uint8 r = n % 10;
        n = n / 10;
        str[i] = 48 + r;
        i++;
    } while (n > 0);

    for (uint8 i = 0; i < 16; i++)
    {
        if (str[15 - i] != 0)
        {
            printChar(str[15 - i]);
        }
    }
}

void print(uint8 *str)
{
    uint8 i = 0;
    while (str[i] != 0)
    {
        printChar(str[i]);
        i++;
    }
}