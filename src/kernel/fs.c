#include "fs.h"

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
} superblock;

typedef struct
{
    uint32 blockUsageBitmapAddress;
    uint32 inodeUsageBitmapAddress;
    uint32 inodeTableAddress;
    uint16 unallocatedBlocksNumber;
    uint16 unallocatedInodesNumber;
    uint16 directoriesNumber;
} blockGroupDescriptor;

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
} inode;

typedef struct
{
    uint32 inode;
    uint16 size;
    uint8 nameLength;
    uint8 type;
    uint8 *name;
} directory;

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

inode readInode(superblock *superblock, blockGroupDescriptor *block, uint32 number)
{
    number = number - 1;
    uint32 inodeBlock = (number * superblock->inodeSize) / superblock->blockSize;
    uint8 inode_table_buffer[512];
    readDisk((block->inodeTableAddress + inodeBlock) * 8, 1, inode_table_buffer);
    inode i;
    i.typeAndPermissions = *(uint16 *)(inode_table_buffer + superblock->inodeSize * number);
    i.userId = *(uint16 *)(inode_table_buffer + superblock->inodeSize * number + 2);
    i.lowerSize = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 4);
    i.lastAccessTime = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 8);
    i.creationTime = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 12);
    i.lastModifTime = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 16);
    i.deletionTime = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 20);
    i.groupId = *(uint16 *)(inode_table_buffer + superblock->inodeSize * number + 24);
    i.hardLinkCount = *(uint16 *)(inode_table_buffer + superblock->inodeSize * number + 26);
    i.diskSectorsCount = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 28);
    i.flags = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 32);
    i.reserved = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 36);
    i.directPointer0 = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 40);
    i.directPointer1 = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 44);
    i.directPointer2 = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 48);
    i.directPointer3 = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 52);
    i.directPointer4 = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 56);
    i.directPointer5 = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 60);
    i.directPointer6 = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 64);
    i.directPointer7 = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 68);
    i.directPointer8 = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 72);
    i.directPointer9 = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 76);
    i.directPointer10 = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 80);
    i.directPointer11 = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 84);
    i.indirectPointer = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 88);
    i.doubleIndirectPointer = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 92);
    i.tripleIndirectPointer = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 96);
    i.generationNumber = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 100);
    i.extendedAttributeBlock = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 104);
    i.upperSize = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 108);
    i.fragmentAddress = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 112);
    i.reserved2 = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 116);
    i.reserved3 = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 120);
    i.reserved4 = *(uint32 *)(inode_table_buffer + superblock->inodeSize * number + 124);

    return i;
}

superblock readSuperblock()
{
    superblock block;
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

blockGroupDescriptor readBlockGroupDescriptor()
{
    blockGroupDescriptor block;
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

uint8 *readBlock(uint32 index)
{
    uint8 *ptr = kmalloc(4096);
    readDisk(index * 8, 8, ptr);
    return ptr;
}

directory readDirectory(inode *i, uint16 number)
{
    uint8 *data = readBlock(i->directPointer0);
    uint32 padding = 0;
    for (uint16 i = 0; i < number; i++)
    {
        uint16 size = *(uint16 *)(data + padding + 4);
        padding += size;
    }

    directory dir;
    dir.inode = *(uint32 *)(data + padding);
    dir.size = *(uint16 *)(data + padding + 4);
    dir.nameLength = *(uint8 *)(data + padding + 6);
    dir.type = *(uint8 *)(data + padding + 7);
    uint8 *name = kmalloc(dir.nameLength + 1);
    for (uint8 i = 0; i < dir.nameLength; i++)
    {
        name[i] = *(uint8 *)(data + padding + 8 + i);
    }
    name[dir.nameLength] = 0;
    dir.name = name;
    kfree(data);
    return dir;
}

void fs_start()
{

    // printStatus();
    // waitDiskReady();
    // printStatus();

    // uint32 buff[256];
    // for (uint16 i = 0; i < 256; i++)
    // {
    //     buff[i] = 0x00;
    // }

    // readDisk(2, 2, buff);

    // uint32 blockSize = 1024 << buff[6];
    // uint32 numberOfInode = buff[0];
    // uint32 numberOfBlock = buff[1];
    // uint32 blockPerBlockGroup = buff[8];
    // uint32 inodePerBlockGroup = buff[10];
    // uint8 numberOfBlockGroup = (numberOfBlock / blockPerBlockGroup) + ((numberOfBlock % blockPerBlockGroup == 0) ? 0 : 1);
    // if ((numberOfInode / inodePerBlockGroup) + ((numberOfInode % inodePerBlockGroup == 0) ? 0 : 1) != numberOfBlockGroup)
    // {
    //     panic("Error while calculting number of block groups");
    // }

    superblock super = readSuperblock();
    waitDiskReady();
    blockGroupDescriptor blockDescriptor = readBlockGroupDescriptor();
    waitDiskReady();
    inode i = readInode(&super, &blockDescriptor, 2);
    waitDiskReady();
    directory dir = readDirectory(&i, 0);
    print(dir.name);
    printLn();
    waitDiskReady();
    dir = readDirectory(&i, 1);
    print(dir.name);
    printLn();
    waitDiskReady();
    dir = readDirectory(&i, 2);
    print(dir.name);
    printLn();
    waitDiskReady();
    dir = readDirectory(&i, 3);
    print(dir.name);
    printLn();
    waitDiskReady();
    dir = readDirectory(&i, 4);
    print(dir.name);
    printLn();

    // print("block size: ");
    // printInt(blockSize);
    // printLn();
    // print("number of inode: ");
    // printInt(numberOfInode);
    // printLn();
    // print("number of block: ");
    // printInt(numberOfBlock);
    // printLn();
    // print("block / block group: ");
    // printInt(blockPerBlockGroup);
    // printLn();
    // print("inode / block group: ");
    // printInt(inodePerBlockGroup);
    // printLn();
    // print("number of block group: ");
    // printInt(numberOfBlockGroup);
    // printLn();

    // uint16 versionMinor = *((uint16 *)buff + 31);
    // uint32 versionMajor = buff[19];

    // uint32 inodeSize = buff[22];
    // waitDiskReady();

    // uint32 block_group_descriptor[128];
    // readDisk(8, 1, block_group_descriptor);
    // uint32 inode_table_address = block_group_descriptor[2];

    // uint32 block_of_inode = (1 * 128) / blockSize;

    // waitDiskReady();

    // uint8 inode_table_buffer[512];
    // readDisk((inode_table_address + block_of_inode) * 8, 1, inode_table_buffer);

    // for (uint16 i = 128; i < 256; i++)
    // {
    //     print8bitHex(inode_table_buffer[i]);
    //     if (i % 2 != 0)
    //         printChar(' ');
    // }
    // print("size: ");
    // printInt(*((uint32 *)(inode_table_buffer + 128 + 4)));
    // printLn();
    // printHex(*((uint16 *)(inode_table_buffer + 128)));
    // printLn();
    // printHex(*((uint32 *)(inode_table_buffer + 128 + 40)));

    // waitDiskReady();
    // uint8 *dataBlock = (uint8 *)kmalloc();
    // uint32 x = *((uint32 *)(inode_table_buffer + 128 + 40)) * 8;
    // // printHex((*((uint32 *)(inode_table_buffer + 128 + 40)) * 8) * 512);
    // readDisk(0x146000 / 512, 8, dataBlock);

    // printInt(*((uint32 *)(dataBlock)));
    // printLn();
    // printInt(*((uint16 *)(dataBlock + 4)));
    // printLn();
    // printInt(*(dataBlock + 6));
    // printLn();
    // printInt(*(dataBlock + 7));
    // printLn();
    // print(dataBlock + 8);
    // printLn();

    // printInt(*(uint16 *)(dataBlock + 28));
    // printLn();
    // print(dataBlock + 32 + 44);
}

void readDisk(uint32 startSector, uint16 nbSector, void *buff)
{
    write_port(0x1F2, nbSector);
    write_port(0x1F3, (uint8)startSector);
    write_port(0x1F4, (uint8)(startSector >> 8));
    write_port(0x1F5, (uint8)(startSector >> 16));
    write_port(0x1F6, (uint8)((startSector >> 24) | 0b11100000));
    write_port(0x1F7, 0x20);

    uint8 i = 0;

    while (i < nbSector)
    {
        while (!(read_port(0x1F7) & 8))
        {
        }
        insw(0x1F0, (buff + i * 512), 256);
        i++;
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