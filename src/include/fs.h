#pragma once

#include "types.h"

namespace Fs
{
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

    void init();

    void findEntry(DirectoryEntry *directoryEntry, const char *path);
    Inode readInode(uint32 number);
    uint32 readInodeData(const Inode *inode, void *buff, uint32 offset, uint32 size);

} // namespace Fs
