#include "processes.h"
#include "memory.h"
#include "panic.h"
#include "terminal.h"

ProcessesQueue::ProcessesQueue()
{
    uint32 address = Memory::Physical::get_free_pages(1);
    m_data = Memory::Pages::getKernelFreePage();
    Memory::Pages::mapPage(m_data, address, false);
    m_maxData = m_data + 4096;

    m_popIndex = m_data;
    m_pushIndex = m_data;
}

void ProcessesQueue::push(Process *process)
{

    if (++m_size > 1024)
        panic("Processes queue: cannot push: full");

    *(uint32 *)m_pushIndex = (uint32)process;
    m_pushIndex += 4;
    if (m_pushIndex == m_maxData)
    {
        m_pushIndex = m_data;
    }
}

Process *ProcessesQueue::pop()
{

    if (m_size-- == 0)
        panic("Processes queue: cannot pop: empty");

    Process *process = (Process *)(*(uint32 *)m_popIndex);
    m_popIndex += 4;
    if (m_popIndex == m_maxData)
    {
        m_popIndex = m_data;
    }
    return process;
}

Process *ProcessesQueue::first()
{
    return (Process *)(*(uint32 *)m_popIndex);
}

uint32 ProcessesQueue::getSize()
{
    return m_size;
}