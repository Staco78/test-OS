#include "terminal.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

uint16 index = 0;

void printChar(uint8 c)
{
    uint8 *ptr = (uint8 *)0xB8000;
    ptr[index++ * 2] = c;
    if (index > SCREEN_HEIGHT * SCREEN_WIDTH)
        index = 0;
}

void printInt(uint32 x)
{
    uint32 n = x;
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

// void print(uint8 *str)
// {
//     uint8 i = 0;
//     // while (str[i] != 0)
//     // {
//     //     printInt(i);
//     //     // printChar(str[i]);
//     //     i++;
//     // }
//     printInt(i);
// }