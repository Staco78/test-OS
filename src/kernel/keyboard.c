#include "keyboard.h"

uint8 isCaps = 0;

void keyboard_interrupt()
{

    uint8 keyboard_map[128] =
        {
            0, 27, '&', 'e', '"', '\'', '(', '-', 'e', '_',   /* 9 */
            'c', 'a', ')', '=', '\b',                         /* Backspace */
            '\t',                                             /* Tab */
            'a', 'z', 'e', 'r',                               /* 19 */
            't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n',     /* Enter key */
            0,                                                /* 29   - Control */
            'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', /* 39 */
            'u', 0, 0,                                        /* Left shift */
            '*', 'w', 'x', 'c', 'v', 'b', 'n',                /* 49 */
            ',', ';', ':', '!', 0,                            /* Right shift */
            '*',
            0,   /* Alt */
            ' ', /* Space bar */
            0,   /* Caps lock */
            0,   /* 59 - F1 key ... > */
            0, 0, 0, 0, 0, 0, 0, 0,
            0, /* < ... F10 */
            0, /* 69 - Num lock*/
            0, /* Scroll Lock */
            0, /* Home key */
            0, /* Up Arrow */
            0, /* Page Up */
            '-',
            0, /* Left Arrow */
            0,
            0, /* Right Arrow */
            '+',
            0, /* 79 - End key*/
            0, /* Down Arrow */
            0, /* Page Down */
            0, /* Insert Key */
            0, /* Delete Key */
            0, 0, 0,
            0, /* F11 Key */
            0, /* F12 Key */
            0, /* All other keys are undefined */
        };

    uint8 caps_map[128] = {
        0, 27, '1', '2', '3', '4', '5', '6', '7', '8',    /* 9 */
        '9', '0', 0, '+', '\b',                           /* Backspace */
        '\t',                                             /* Tab */
        'A', 'Z', 'E', 'R',                               /* 19 */
        'T', 'Y', 'U', 'I', 'O', 'P', 0, 0, '\n',         /* Enter key */
        0,                                                /* 29   - Control */
        'Q', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', /* 39 */
        0, 0, 0,                                          /* Left shift */
        0, 'W', 'X', 'C', 'V', 'B', 'N',                  /* 49 */
        '?', '.', '/', 0, 0,                              /* Right shift */
        0,
        0,   /* Alt */
        ' ', /* Space bar */
        0,   /* Caps lock */
        0,   /* 59 - F1 key ... > */
        0, 0, 0, 0, 0, 0, 0, 0,
        0, /* < ... F10 */
        0, /* 69 - Num lock*/
        0, /* Scroll Lock */
        0, /* Home key */
        0, /* Up Arrow */
        0, /* Page Up */
        '-',
        0, /* Left Arrow */
        0,
        0, /* Right Arrow */
        '+',
        0, /* 79 - End key*/
        0, /* Down Arrow */
        0, /* Page Down */
        0, /* Insert Key */
        0, /* Delete Key */
        0, 0, 0,
        0, /* F11 Key */
        0, /* F12 Key */
        0, /* All other keys are undefined */
    };

    write_port(0x20, 0x20);

    uint8 status = read_port(0x64);

    if (status & 0x01)
    {
        // printInt(status);
        uint8 keycode = read_port(0x60);
        if (keycode == 42 || keycode == 54)
        {
            isCaps = 1;
        }
        else if (keycode == 170 || keycode == 182)
        {
            isCaps = 0;
        }
        else if (keycode <= 128)
        {
            uint8 key;
            if (isCaps == 0)
                key = keyboard_map[keycode];
            else
                key = caps_map[keycode];

            if (key == 0)
            {
                printInt(keycode);
            }
            else
            {
                printChar(key);
            }
        }
    }

    __asm__ volatile("sti");
}
