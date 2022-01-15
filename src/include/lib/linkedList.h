#pragma once
#include "memory.h"
#include "panic.h"

template <typename T>
class LinkedList
{
public:
    void push(T data)
    {
        Node *new_node = (Node *)kmalloc(sizeof(Node));
        new_node->data = data;
        if (last == nullptr)
        {
            head = new_node;
            last = new_node;
        }
        else
        {
            last->next = new_node;
            last = new_node;
        }

        size++;
    }

    T &operator[](uint32 index)
    {
        if (index >= size)
            panic("LinkedList: [] index out of range");

        Node *current = head;
        for (int i = 0; i < index; i++)
        {
            current = current->next;
        }
        return current->data;
    }

    void clear()
    {
        Node *current = head;
        for (int i = 0; i < size; i++)
        {
            Node *next = current->next;
            kfree(current);
            current = next;
        }

        head = nullptr;
        last = nullptr;
        size = 0;
    }

    uint32 getSize()
    {
        return size;
    }

    ~LinkedList()
    {
        clear();
    }

private:
    struct Node
    {
        T data;
        Node *next;
    };

    Node *head = nullptr;
    Node *last = nullptr;

    uint32 size = 0;
};