#include "types.h"
#include "idt.h"

// uint8 *video_memory;

// void drawPixel(uint8 color, uint8 x, uint8 y)
// {
//     *(video_memory + x + y * SCREEN_WIDTH) = color;
// }

// void drawRect(uint8 color, uint8 x, uint8 y, uint8 w, uint8 h)
// {
//     uint8 *vram = video_memory + x + y * SCREEN_WIDTH;
//     uint8 i, j;
//     for (i = 0; i < w; i++)
//     {
//         for (j = 0; j < h; j++)
//         {
//             vram[j] = color;
//         }
//         vram += 320;
//     }
// }

// void main()
// {
// video_memory = (uint8 *)0xA0000;
// // clear();
// // char x[6] = "hello";
// // print(x);

// uint8 color = 0;

// for (uint8 x = 0; x < SCREEN_WIDTH / 50; x++)
// {
//     for (uint8 y = 0; y < SCREEN_HEIGHT / 50; y++)
//     {
//         if (color++ == 255)
//         {
//             color = 0;
//         }
//         drawRect(color, x * 50, y * 50, 50, 50);
//     }
// }

// uint16 lastLine = video_memory + (SCREEN_HEIGHT - 1) * SCREEN_WIDTH;
// uint8 line[SCREEN_WIDTH];

// while (1)
// {

//     for (uint16 i = 0; i < SCREEN_WIDTH; i++)
//     {
//         line[i] = video_memory[i];
//     }

//     for (uint16 i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++)
//     {
//         video_memory[i] = video_memory[i + SCREEN_WIDTH];
//     }

//     for (uint16 i = 0; i < SCREEN_WIDTH; i++)
//     {
//         video_memory[lastLine + i] = line[i];
//     }

//     for (uint32 i = 0; i < 1000000; i++)
//     {
//     }
// }
// }


void main()
{
    idt_assemble();

    // uint8 *ptr = (uint8 *)0xB8000;
    // char str[] = "salut";
    // uint8 i = 0;
    // while (str[i])
    // {
    //     ptr[i * 2] = str[i];
    //     i++;
    // }

    // int x = 1 / 0;
}
