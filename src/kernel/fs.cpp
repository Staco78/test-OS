#include "fs.h"
#include "asm.h"
#include "terminal.h"
#include "memory.h"
#include "lib/linkedList.h"
#include "lib/memoryUtils.h"

#define INODE_TYPE_FIFO 0x1000
#define INODE_TYPE_CHARACTER_DEVICE 0x2000
#define INODE_TYPE_DIRECTORY 0x4000
#define INODE_TYPE_BLOCK_DEVICE 0x6000
#define INODE_TYPE_FILE 0x8000
#define INODE_TYPE_SYMBOLIC_LINK 0xA000
#define INODE_TYPE_SOCKET 0xC000

#define DIRECTORY_ENTRY_TYPE_FILE 1
#define DIRECTORY_ENTRY_TYPE_DIRECTORY 2
#define DIRECTORY_ENTRY_TYPE_CHARACTER_DEVICE 3
#define DIRECTORY_ENTRY_TYPE_BLOCK_DEVICE 4
#define DIRECTORY_ENTRY_TYPE_FIFO 5
#define DIRECTORY_ENTRY_TYPE_SOCKET 6
#define DIRECTORY_ENTRY_TYPE_SOFT_LINK 7

void insw(uint16 port, void *addr, unsigned long count)
{
    __asm__ volatile("cli");
    __asm__ volatile("rep; insw"
                     : "+D"(addr), "+c"(count)
                     : "d"(port));
    __asm__ volatile("sti");
}

void outsw(uint16 port, void *addr, unsigned long count)
{
    __asm__ volatile("cli");
    __asm__ volatile("rep; outsw"
                     : "+S"(addr), "+c"(count)
                     : "d"(port));
    __asm__ volatile("sti");
}

typedef struct
{
    uint32 inodesNumber;
    uint32 blocksNumber;
    uint32 reservedBlocks;
    uint32 unallocatedBlocks;
    uint32 unallocatedInodes;
    uint32 superblockBlock;
    uint32 blockSize;
    uint32 fragmentSize;
    uint32 blocksInBlockGroup;
    uint32 fragmentInBlockGroup;
    uint32 inodesInBlockGroup;
    uint32 lastMountTime;
    uint32 lastWrittenTime;
    uint16 mountedSinceLastCheck;
    uint16 mountsBeforeCheck;
    uint16 signature;
    uint16 fsState;
    uint16 errorHandlingMethod;
    uint16 versionMinor;
    uint32 timeSinceLastCheck;
    uint32 intervalBetweenChecks;
    uint32 osId;
    uint32 versionMajor;
    uint16 reservedBlocksUserId;
    uint32 reservedBlocksGroupId;
    uint32 firstNonReservedInode;
    uint16 inodeSize;
    uint16 blockGroup;
    uint32 optionalFeatures;
    uint32 requiredFeatures;
    uint32 writeRequiredFeatures;
} Superblock;

typedef struct
{
    uint32 blockUsageBitmapAddress;
    uint32 inodeUsageBitmapAddress;
    uint32 inodeTableAddress;
    uint16 unallocatedBlocksNumber;
    uint16 unallocatedInodesNumber;
    uint16 directoriesNumber;
} BlockGroupDescriptor;

typedef struct
{
    uint16 typeAndPermissions;
    uint16 userId;
    uint32 lowerSize;
    uint32 lastAccessTime;
    uint32 creationTime;
    uint32 lastModifTime;
    uint32 deletionTime;
    uint16 groupId;
    uint16 hardLinkCount;
    uint32 diskSectorsCount;
    uint32 flags;
    uint32 reserved;
    uint32 directPointer0;
    uint32 directPointer1;
    uint32 directPointer2;
    uint32 directPointer3;
    uint32 directPointer4;
    uint32 directPointer5;
    uint32 directPointer6;
    uint32 directPointer7;
    uint32 directPointer8;
    uint32 directPointer9;
    uint32 directPointer10;
    uint32 directPointer11;
    uint32 indirectPointer;
    uint32 doubleIndirectPointer;
    uint32 tripleIndirectPointer;
    uint32 generationNumber;
    uint32 extendedAttributeBlock;
    uint32 upperSize;
    uint32 fragmentAddress;
    uint32 reserved2;
    uint32 reserved3;
    uint32 reserved4;
} Inode;

typedef struct
{
    uint32 inode;
    uint16 size;
    uint8 nameLength;
    uint8 type;
    char *name;
} DirectoryEntry;

Superblock superblock;
BlockGroupDescriptor blockDescriptor;
Inode rootDirectoryInode;

void waitDiskReady()
{
    uint8 status;
    do
    {
        status = read_port(0x1F7);
    } while (status & 0b10000000 || !(status & 0b01000000));
}

void printStatus()
{
    printBin(read_port(0x1F7));
    printLn();
}

Inode readInode(uint32 number)
{
    number -= 1;
    uint32 inodeBlock = (number * superblock.inodeSize) / superblock.blockSize;
    uint8 *inode_table_buffer = (uint8 *)kmalloc(4096);
    readDisk((blockDescriptor.inodeTableAddress + inodeBlock) * 8, 8, inode_table_buffer);
    Inode inode;
    uint32 baseOffset = (uint32)inode_table_buffer + superblock.inodeSize * number;
    inode.typeAndPermissions = *(uint16 *)(baseOffset);
    inode.userId = *(uint16 *)(baseOffset + 2);
    inode.lowerSize = *(uint32 *)(baseOffset + 4);
    inode.lastAccessTime = *(uint32 *)(baseOffset + 8);
    inode.creationTime = *(uint32 *)(baseOffset + 12);
    inode.lastModifTime = *(uint32 *)(baseOffset + 16);
    inode.deletionTime = *(uint32 *)(baseOffset + 20);
    inode.groupId = *(uint16 *)(baseOffset + 24);
    inode.hardLinkCount = *(uint16 *)(baseOffset + 26);
    inode.diskSectorsCount = *(uint32 *)(baseOffset + 28);
    inode.flags = *(uint32 *)(baseOffset + 32);
    inode.reserved = *(uint32 *)(baseOffset + 36);
    inode.directPointer0 = *(uint32 *)(baseOffset + 40);
    inode.directPointer1 = *(uint32 *)(baseOffset + 44);
    inode.directPointer2 = *(uint32 *)(baseOffset + 48);
    inode.directPointer3 = *(uint32 *)(baseOffset + 52);
    inode.directPointer4 = *(uint32 *)(baseOffset + 56);
    inode.directPointer5 = *(uint32 *)(baseOffset + 60);
    inode.directPointer6 = *(uint32 *)(baseOffset + 64);
    inode.directPointer7 = *(uint32 *)(baseOffset + 68);
    inode.directPointer8 = *(uint32 *)(baseOffset + 72);
    inode.directPointer9 = *(uint32 *)(baseOffset + 76);
    inode.directPointer10 = *(uint32 *)(baseOffset + 80);
    inode.directPointer11 = *(uint32 *)(baseOffset + 84);
    inode.indirectPointer = *(uint32 *)(baseOffset + 88);
    inode.doubleIndirectPointer = *(uint32 *)(baseOffset + 92);
    inode.tripleIndirectPointer = *(uint32 *)(baseOffset + 96);
    inode.generationNumber = *(uint32 *)(baseOffset + 100);
    inode.extendedAttributeBlock = *(uint32 *)(baseOffset + 104);
    inode.upperSize = *(uint32 *)(baseOffset + 108);
    inode.fragmentAddress = *(uint32 *)(baseOffset + 112);
    inode.reserved2 = *(uint32 *)(baseOffset + 116);
    inode.reserved3 = *(uint32 *)(baseOffset + 120);
    inode.reserved4 = *(uint32 *)(baseOffset + 124);

    kfree(inode_table_buffer);
    return inode;
}

Superblock readSuperblock()
{
    Superblock block;
    uint8 buff[1024];
    readDisk(2, 2, buff);

    block.inodesNumber = *(uint32 *)(buff + 0);
    block.blocksNumber = *(uint32 *)(buff + 4);
    block.reservedBlocks = *(uint32 *)(buff + 8);
    block.unallocatedBlocks = *(uint32 *)(buff + 12);
    block.unallocatedInodes = *(uint32 *)(buff + 16);
    block.superblockBlock = *(uint32 *)(buff + 20);
    block.blockSize = *(uint32 *)(buff + 24);
    block.fragmentSize = *(uint32 *)(buff + 28);
    block.blocksInBlockGroup = *(uint32 *)(buff + 32);
    block.fragmentInBlockGroup = *(uint32 *)(buff + 36);
    block.inodesInBlockGroup = *(uint32 *)(buff + 40);
    block.lastMountTime = *(uint32 *)(buff + 44);
    block.lastWrittenTime = *(uint32 *)(buff + 48);
    block.mountedSinceLastCheck = *(uint16 *)(buff + 52);
    block.mountsBeforeCheck = *(uint16 *)(buff + 54);
    block.signature = *(uint16 *)(buff + 56);
    block.fsState = *(uint16 *)(buff + 58);
    block.errorHandlingMethod = *(uint16 *)(buff + 60);
    block.versionMinor = *(uint16 *)(buff + 62);
    block.timeSinceLastCheck = *(uint16 *)(buff + 64);
    block.intervalBetweenChecks = *(uint32 *)(buff + 68);
    block.osId = *(uint32 *)(buff + 72);
    block.versionMajor = *(uint32 *)(buff + 76);
    block.reservedBlocksUserId = *(uint16 *)(buff + 80);
    block.reservedBlocksGroupId = *(uint16 *)(buff + 82);
    block.firstNonReservedInode = *(uint32 *)(buff + 84);
    block.inodeSize = *(uint16 *)(buff + 88);
    block.blockGroup = *(uint16 *)(buff + 90);
    block.optionalFeatures = *(uint32 *)(buff + 92);
    block.requiredFeatures = *(uint32 *)(buff + 96);
    block.writeRequiredFeatures = *(uint32 *)(buff + 100);

    block.blockSize = 1024 << block.blockSize;
    block.fragmentSize = 1024 << block.fragmentSize;
    return block;
}

BlockGroupDescriptor readBlockGroupDescriptor()
{
    BlockGroupDescriptor block;
    uint8 buff[512];
    readDisk(8, 1, buff);

    block.blockUsageBitmapAddress = *(uint32 *)(buff + 0);
    block.inodeUsageBitmapAddress = *(uint32 *)(buff + 4);
    block.inodeTableAddress = *(uint32 *)(buff + 8);
    block.unallocatedBlocksNumber = *(uint16 *)(buff + 12);
    block.unallocatedInodesNumber = *(uint16 *)(buff + 14);
    block.directoriesNumber = *(uint16 *)(buff + 16);

    return block;
}

inline void readBlock(uint32 index, void *buff)
{
    readDisk(index * (superblock.blockSize / 512), (superblock.blockSize / 512), buff);
}

void readDirectory(Inode *inode, LinkedList<DirectoryEntry> &list)
{
    uint32 size = inode->lowerSize;
    uint8 *data = (uint8 *)kmalloc(superblock.blockSize);
    readBlock(inode->directPointer0, data);

    uint32 padding = 0;
    uint32 i = 0;
    for (; padding < size; i++)
    {
        list.push({});
        list[i].inode = *(uint32 *)(data + padding);
        list[i].size = *(uint16 *)(data + padding + 4);
        if (list[i].size == 0)
            break;
        list[i].nameLength = *(uint8 *)(data + padding + 6);
        list[i].type = *(uint8 *)(data + padding + 7);
        char *name = (char *)kmalloc(list[i].nameLength + 1);
        for (uint8 j = 0; j < list[i].nameLength; j++)
        {
            name[j] = *(uint8 *)(data + padding + 8 + j);
        }
        name[list[i].nameLength] = 0;
        list[i].name = name;
        padding += list[i].size;
    }

    kfree(data);
}

uint32 getBlockPointer(const Inode *inode, uint16 index)
{

    if (index < 12)
    {
        switch (index)
        {
        case 0:
            return inode->directPointer0;

        case 1:
            return inode->directPointer1;

        case 2:
            return inode->directPointer2;

        case 3:
            return inode->directPointer3;

        case 4:
            return inode->directPointer4;

        case 5:
            return inode->directPointer5;

        case 6:
            return inode->directPointer6;

        case 7:
            return inode->directPointer7;

        case 8:
            return inode->directPointer8;

        case 9:
            return inode->directPointer9;

        case 10:
            return inode->directPointer10;

        case 11:
            return inode->directPointer11;
        }
    }
    else
    {
        index += 12;
        if (index < (superblock.blockSize / 4))
        {
            // singly indirect block pointer
            uint32 *data = (uint32 *)kmalloc(superblock.blockSize);
            readBlock(inode->indirectPointer, data);
            uint32 ptr = data[index];
            kfree(data);
            return ptr;
        }
        else if (index < (superblock.blockSize / 4) * (superblock.blockSize / 4))
        {
            // doubly indirect block pointer
            uint32 *data = (uint32 *)kmalloc(superblock.blockSize);
            readBlock(inode->doubleIndirectPointer, data);
            uint32 indirectBlockIndex = data[index / (superblock.blockSize / 4)];
            readBlock(indirectBlockIndex, data);
            uint32 doubleIndirectPointer = data[index % (superblock.blockSize / 4)];
            kfree(data);
            return doubleIndirectPointer;
        }
        else if (index < (superblock.blockSize / 4) * (superblock.blockSize / 4) * (superblock.blockSize / 4))
        {
            // triply indirect block pointer
            uint32 *data = (uint32 *)kmalloc(superblock.blockSize);
            readBlock(inode->doubleIndirectPointer, data);
            uint32 indirectBlockIndex = data[index / (superblock.blockSize / 4) / (superblock.blockSize / 4)];
            readBlock(indirectBlockIndex, data);
            uint32 doubleIndirectPointer = data[(index / (superblock.blockSize / 4)) % (superblock.blockSize / 4)];
            readBlock(doubleIndirectPointer, data);
            uint32 tripleIndirectPointer = data[index % (superblock.blockSize / 4)];
            kfree(data);
            return tripleIndirectPointer;
        }
    }
    panic("Invalid block index");
    return 0;
}

uint32 readInodeData(const Inode *inode, void *buff, uint32 offset, uint32 size)
{
    uint32 fileSize = inode->lowerSize;
    if (offset + size > fileSize)
        return 0;

    uint16 blockIndex = offset / superblock.blockSize;
    uint32 sizeToRead = size;
    uint32 buffOffset = 0;

    if (offset % superblock.blockSize != 0)
    {
        void *data = kmalloc(superblock.blockSize);
        readBlock(getBlockPointer(inode, blockIndex), data);
        memcpy(buff, (uint8 *)data + offset % superblock.blockSize, superblock.blockSize - (offset % superblock.blockSize));
        sizeToRead -= (offset % superblock.blockSize);
        buffOffset += (offset % superblock.blockSize);
        kfree(data);
        blockIndex++;
    }
    while (sizeToRead > 0)
    {
        if (sizeToRead >= superblock.blockSize)
        {
            readBlock(getBlockPointer(inode, blockIndex++), (uint8 *)buff + buffOffset);
            sizeToRead -= superblock.blockSize;
            buffOffset += superblock.blockSize;
        }
        else
        {
            void *data = kmalloc(superblock.blockSize);
            readBlock(getBlockPointer(inode, blockIndex++), data);
            memcpy((uint8 *)buff + buffOffset, data, sizeToRead);
            kfree(data);
            buffOffset += sizeToRead;
            sizeToRead = 0;
        }
    }

    return buffOffset;
}

void findDirectoryEntryFromPath(const Inode *baseInode, DirectoryEntry *directoryEntry, char *path, uint16 pathOffset)
{
    if (path[pathOffset] == '/')
        pathOffset++;

    uint32 lastPathOffset = pathOffset;

    while (path[pathOffset] && path[pathOffset] != '/')
    {
        pathOffset++;
    }

    uint32 offset = 0;
    uint32 read = 0;
    uint8 *data = (uint8 *)kmalloc(superblock.blockSize);
loop:
    if (offset >= baseInode->lowerSize)
    {
        kfree(data);
        panic("fs: file not found");
    }
    if (read == offset)
    {
        readInodeData(baseInode, data, offset, superblock.blockSize);
        read += superblock.blockSize;
    }
    uint8 nameLength = *(uint8 *)(data + offset + 6);
    uint32 temp = offset;
    if (nameLength != pathOffset - lastPathOffset)
    {
        offset += *(uint16 *)(data + offset + 4);
        goto loop;
    }
    for (uint8 i = 0; i < nameLength; i++)
    {
        if (*(uint8 *)(data + temp + 8 + i) != path[lastPathOffset + i])
        {
            offset += *(uint16 *)(data + offset + 4);
            goto loop;
        }
    }

    directoryEntry->inode = *(uint32 *)(data + offset);
    directoryEntry->size = *(uint16 *)(data + offset + 4);
    directoryEntry->nameLength = *(uint8 *)(data + offset + 6);
    directoryEntry->type = *(uint8 *)(data + offset + 7);
    char *name = (char *)kmalloc(directoryEntry->nameLength + 1);
    for (uint8 j = 0; j < directoryEntry->nameLength; j++)
    {
        name[j] = *(uint8 *)(data + offset + 8 + j);
    }
    name[directoryEntry->nameLength] = 0;
    directoryEntry->name = name;

    kfree(data);
    if (directoryEntry->type == DIRECTORY_ENTRY_TYPE_DIRECTORY)
    {
        Inode inode = readInode(directoryEntry->inode);
        findDirectoryEntryFromPath(&inode, directoryEntry, path, pathOffset);
        return;
    }
    return;
}

void identify_disk();

void fs_start()
{
    identify_disk();

    superblock = readSuperblock();
    blockDescriptor = readBlockGroupDescriptor();
    rootDirectoryInode = readInode(2);

    DirectoryEntry file;
    findDirectoryEntryFromPath(&rootDirectoryInode, &file, "/coucou", 0);
    Inode fileInode = readInode(file.inode);
    char* data = (char*)kmalloc(fileInode.lowerSize);
    readInodeData(&fileInode, data, 0, fileInode.lowerSize);
    print(data, fileInode.lowerSize);
    kfree(data);
}

void identify_disk()
{
    uint8 *buff = (uint8 *)kmalloc(512);
    write_port(0x1F2, 0);
    write_port(0x1F3, 0);
    write_port(0x1F4, 0);
    write_port(0x1F5, 0);
    write_port(0x1F6, 0xA0);
    write_port(0x1F7, 0xEC);

    insw(0x1F0, buff, 256);

    kfree(buff);
}

void readDisk(uint32 startSector, uint16 nbSector, void *buff)
{
    waitDiskReady();

    write_port(0x1F2, nbSector);
    write_port(0x1F3, (uint8)startSector);
    write_port(0x1F4, (uint8)(startSector >> 8));
    write_port(0x1F5, (uint8)(startSector >> 16));
    write_port(0x1F6, (uint8)((startSector >> 24) | 0b11100000));
    write_port(0x1F7, 0x20);

    uint8 inode = 0;

    while (inode < nbSector)
    {
        while (!(read_port(0x1F7) & 8))
        {
        }
        insw(0x1F0, ((uint8 *)buff + inode * 512), 256);
        inode++;
    }
}

void writeDisk(uint32 startSector, uint16 nbSector, void *buff)
{
    write_port(0x1F6, (startSector >> 24) | 0b11100000);
    write_port(0x1F2, nbSector);
    write_port(0x1F3, (uint8)startSector);
    write_port(0x1F4, (uint8)startSector >> 8);
    write_port(0x1F5, (uint8)startSector >> 16);
    write_port(0x1F7, 0x30);

    while (!(read_port(0x1F7) & 8))
    {
    }

    outsw(0x1F0, buff, 256 * nbSector);
}