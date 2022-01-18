#include "memory.h"
#include "panic.h"
#include "terminal.h"

namespace Memory
{
    namespace Physical
    {
        struct memory_map
        {
            uint32 address;
            uint32 length;
            uint32 type;
        };

        struct memory_map memory_maps[6];

        uint8 *memory_bitmap;

        void set_1(uint32 index)
        {
            uint8 *bitmap = &memory_bitmap[index / 8];
            *bitmap = *bitmap | (0b10000000 >> (index % 8));
        }

        void set_0(uint32 index)
        {
            uint8 *bitmap = &memory_bitmap[index / 8];
            *bitmap = *bitmap & ~(0b10000000 >> (index % 8));
        }

        int get(uint32 index)
        {
            uint8 *bitmap = &memory_bitmap[index / 8];
            return *bitmap & (0b10000000 >> (index % 8));
        }

        // return physical address of count free pages
        uint32 find_free_pages(int count, uint32 alignment)
        {
            uint32 index;
            int size = 0;
            for (uint32 i = 0; i < 131072 * 8; i++)
            {
                if (get(i) == 0)
                {
                    if (size == 0)
                    {
                        if ((i * 4096) % alignment == 0)
                            index = i;
                        else
                            continue;
                    }
                    size++;
                    if (size == count)
                        return index * 4096;
                }
                else
                    size = 0;
            }
            panic("Physical memory allocator: cannot find free pages");
            return 0;
        }

        // return physical address of count free pages and mark they used
        uint32 get_free_pages_aligned(int count, uint32 alignment)
        {
            if (alignment % 4096 != 0)
                panic("Physical memory allocator: invalid alignment");
            uint32 address = find_free_pages(count, alignment);
            for (uint32 i = address / 4096; i < address / 4096 + count; i++)
                set_1(i);
            return address;
        }

        // return physical address of count free pages and mark they used
        uint32 get_free_pages(int count)
        {
            return get_free_pages_aligned(count, 4096);
        }

        void init()
        {
            memory_bitmap = (uint8 *)0xC0500000;
            uint8 *ptr = (uint8 *)0xC000F000;
            for (uint8 i = 0; i < 6; i++)
            {
                memory_maps[i].address = (ptr[24 * i + 4]) + (ptr[24 * i + 5] << 8) + (ptr[24 * i + 6] << 16) + (ptr[24 * i + 7] << 24);
                memory_maps[i].length = (ptr[24 * i + 12]) + (ptr[24 * i + 13] << 8) + (ptr[24 * i + 14] << 16) + (ptr[24 * i + 15] << 24);
                memory_maps[i].type = (ptr[24 * i + 20]) + (ptr[24 * i + 21] << 8) + (ptr[24 * i + 22] << 16) + (ptr[24 * i + 23] << 24);
            }

            // fill bitmap with 1
            for (int i = 0; i < 131072; i++)
            {
                memory_bitmap[i] = 0xFF;
            }
            // __asm__("xchgw %bx, %bx");

            for (uint8 i = 0; i < 6; i++)
            {
                // __asm__("xchgw %bx, %bx");
                if (memory_maps[i].type == 1)
                {
                    uint16 n = memory_maps[i].length / 4096;
                    for (int j = memory_maps[i].address / 4096; j < n + (memory_maps[i].address / 4096); j++)
                    {
                        if (j * 4096 >= 0x100000)
                        {
                            set_0(j);
                        }
                    }
                }
            }

            for (int i = 32; i < 164; i++)
            {
                memory_bitmap[i] = 0xFF;
            }
        }
    }
}