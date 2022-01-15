#pragma once
#include "memory.h"
#include "lib/memoryUtils.h"
#include "terminal.h"

class String
{
public:
    String(const char *str)
    {
        head = (node *)kmalloc(sizeof(node));
        last = head;
        uint32 length = 0;
        while (str[length++])
        {
        }
        head->data = (char *)kmalloc(sizeof(char) * length);
        memcpy(head->data, str, length);

        totalLength = length;
        size = 1;
    }

    ~String()
    {
        clear();
    }

    void clear()
    {
        node *current = head;
        for (int i = 0; i < size; i++)
        {
            node *next = current->next;
            kfree(current->data);
            kfree(current);
            current = next;
        }

        head = nullptr;
        last = nullptr;
        size = 0;
        totalLength = 0;
    }

    uint32 length()
    {
        return totalLength;
    }

private:
    struct node
    {
        char *data;
        struct node *next;
    };

    node *head = nullptr;
    node *last = nullptr;

    uint32 totalLength;
    uint32 size;
};