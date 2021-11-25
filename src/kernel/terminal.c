#include "terminal.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

uint8 *memory_address;

uint8 cursorX = 0;
uint8 cursorY = 0;

void _printChar(uint8 c)
{
    memory_address[(cursorY * SCREEN_WIDTH + cursorX) * 2] = c;
}

void terminalInit()
{
    memory_address = (uint8 *)0xB8000;
    clear();
}

void clearLine(uint8 line)
{
    for (uint8 i = 0; i < SCREEN_WIDTH; i++)
    {
        memory_address[(line * SCREEN_WIDTH + i) * 2] = 0;
        memory_address[(line * SCREEN_WIDTH + i) * 2 + 1] = 0x02;
    }
}

void clear()
{
    for (uint8 i = 0; i < SCREEN_HEIGHT; i++)
    {
        clearLine(i);
    }
}

void scrollLine()
{
    cursorY--;
    for (uint16 i = 0; i < (SCREEN_WIDTH - 1) * SCREEN_HEIGHT * 2; i++)
    {
        memory_address[i] = memory_address[(i + SCREEN_WIDTH * 2)];
    }

    clearLine(SCREEN_HEIGHT - 1);
}

uint8 getChar()
{
    return memory_address[(cursorY * SCREEN_WIDTH + cursorX) * 2];
}

void printChar(uint8 c)
{

    if (c == '\n')
    {
        cursorX = 0;
        cursorY++;
    }
    else if (c == '\b')
    {
        if (cursorX == 0 && cursorY == 0)
        {
            return;
        }
        if (cursorX > 0)
        {
            cursorX--;
            _printChar(0);
        }
        else if (memory_address[(cursorY * SCREEN_WIDTH - 1) * 2] != 0)
        {
            memory_address[(cursorY * SCREEN_WIDTH - 1) * 2] = 0;
            cursorY--;
            cursorX = SCREEN_WIDTH - 1;
        }
        else
        {
            cursorY--;
            cursorX = SCREEN_WIDTH - 1;

            while (getChar() == 0)
            {
                if (cursorX == 0)
                {
                    cursorX = -1;
                    break;
                }
                cursorX--;
            }
            cursorX++;
        }
    }
    else
    {
        _printChar(c);
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

void printLn()
{
    printChar('\n');
}

void print8bitHex(uint8 n)
{
    uint8 i = 0;
    uint8 str[2] = {0};

    while (i < 2)
    {
        uint8 r = n % 16;
        n = n / 16;
        if (r > 9)
        {
            str[i] = 55 + r;
        }
        else
            str[i] = 48 + r;
        i++;
    }

    for (i = 0; i < 2; i++)
    {
        printChar(str[1 - i]);
    }
}

void print32Hex(uint32 n)
{
    for (uint8 i = 0; i < 4; i++)
    {
        print8bitHex((uint8)(n >> (i * 8)));
    }
}

void printCustomHex(uint32 *data)
{
    print32Hex(*(data + 4));
    print32Hex(*data);
    printChar(' ');

    data += 8;

    print32Hex(*(data + 4));
    print32Hex(*data);
    printChar(' ');

    data += 8;

    print32Hex(*data);
    printChar(' ');
    data += 4;
    print32Hex(*data);
}

void printHex(uint32 n)
{
    uint8 i = 0;
    uint8 str[16] = {
        0,
    };

    do
    {
        uint8 r = n % 16;
        n = n / 16;
        if (r > 9)
        {
            str[i] = 55 + r;
        }
        else
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

void printBin(uint8 n)
{
    uint8 i = 0;
    uint8 str[8];

    for (i = 0; i < 8; i++)
        str[i] = 0;

    i = 0;

    do
    {
        uint8 r = n % 2;
        n = n / 2;
        str[i] = 48 + r;
        i++;
    } while (i < 8);

    for (i = 0; i < 8; i++)
    {
        printChar(str[7 - i]);
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