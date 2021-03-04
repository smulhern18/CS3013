//
// Created by Sullivan Mulhern on 2/26/21.
//
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "goatmalloc.h"

#define FREE 1
#define TAKEN 0

size_t totalMemorySize = -1;
node_t* _arena_start;

int statusno = ERR_UNINITIALIZED;


extern int init(size_t size) {
    printf("   requested size %d bytes\n", (int)size);
    if (size > MAX_ARENA_SIZE) {
        printf("   error: requested size larger than MAX_ARENA_SIZE (%d)\n", MAX_ARENA_SIZE);
        return ERR_BAD_ARGUMENTS;
    } else if (size < 1) {
        printf("   error: requested size is negative\n");
        return ERR_BAD_ARGUMENTS;
    }
    int pageSize = getpagesize();
    printf("   pageSize is %d bytes\n", pageSize);
    int remainder = size%pageSize;
    int dividend = size/pageSize;
    if (remainder != 0) {
        dividend++;
    }
    totalMemorySize = pageSize*dividend;
    printf("   adjusted size is %d bytes\n", (int)totalMemorySize);
    int fd = open("/dev/zero",O_RDWR);
    _arena_start = mmap(NULL, totalMemorySize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    printf("   arena starts at %p\n", &_arena_start);
    printf("   arena ends at %p\n", (&_arena_start+totalMemorySize));

    printf("   initializing header for initial free chunk\n");
    _arena_start->size = totalMemorySize-sizeof(node_t);
    _arena_start->bwd = NULL;
    _arena_start->fwd = NULL;
    _arena_start->is_free = FREE;
    printf("   header size is %lu bytes\n", sizeof(node_t));
    statusno = 0;
    return totalMemorySize;
}

extern int destroy() {
    if (statusno != ERR_UNINITIALIZED) {
        printf("   unmapping arena with munmap()\n");
        munmap(_arena_start, totalMemorySize);
        totalMemorySize = -1;
        statusno = ERR_UNINITIALIZED;
        return 0;
    } else {
        return ERR_UNINITIALIZED;
    }
}

extern void* walloc(size_t size){
    printf("   looking for a free chunk of >= %d bytes\n", (int) size);
    if (statusno == ERR_UNINITIALIZED) {
        return NULL;
    } else if (totalMemorySize <= size) {
        printf("   no such free chunk can exist in the memory provided!\n");
        statusno = ERR_OUT_OF_MEMORY;
        return NULL;
    }

    char* incrimentableArenaStart = (char*) _arena_start;
    node_t* currentNode  = _arena_start;
    int hasSpace = 0;
    int bytesMoved = 0;

    while(currentNode != NULL && hasSpace == 0) {
        if (currentNode->is_free == FREE && currentNode->size >= size) {
            printf("   found a free chunk of %d bytes with a header at %p\n", (int) currentNode->size, incrimentableArenaStart+bytesMoved);
            if (currentNode->size > size) {
                //  split
                size_t freeSpace = currentNode->size - (size + 32);
                currentNode->is_free = TAKEN;
                currentNode->size = size;
                node_t* nextNode = (node_t*)(incrimentableArenaStart+size+32+8);
                nextNode->size = freeSpace;
                nextNode->is_free = FREE;
                nextNode->bwd = (struct __node_t *) &currentNode;
                currentNode->fwd = &nextNode;
                bytesMoved += (sizeof(node_t)+size);
            } else {
                // don't split
                currentNode->is_free = TAKEN;
                bytesMoved += 32;
            }
            hasSpace = 1;
        } else {
            bytesMoved += (sizeof(node_t) + currentNode->size);
            currentNode = currentNode->fwd;
        }
    }

    if (hasSpace == 0) {
        statusno = ERR_OUT_OF_MEMORY;
        return NULL;
    }
    printf("   Allocation starts at %p\n", incrimentableArenaStart+bytesMoved);
    return (void*) (incrimentableArenaStart+=bytesMoved);

}

extern void wfree(void *ptr){
    printf("   supplied pointer %p\n", ptr);
    node_t* nodeOfPointer = (node_t*) (((char*) ptr) - 32);
    printf("   accessing chunk header at %p\n", nodeOfPointer);
    printf("   chunk of size of %d\n", (int)nodeOfPointer->size);
    if (nodeOfPointer->fwd != NULL && nodeOfPointer->fwd->is_free) {
        printf(" Adding forward Node to the current node!\n");
        node_t* fwdNode = nodeOfPointer->fwd;
        nodeOfPointer->size += (32 + fwdNode->size);
        nodeOfPointer->fwd = nodeOfPointer->fwd;
        nodeOfPointer->is_free = FREE;
    }
    if (nodeOfPointer->bwd != NULL && nodeOfPointer->bwd->is_free) {
        printf("   Adding current node to the backwards node!\n");
        node_t *bwdNode = nodeOfPointer->bwd;
        bwdNode->size += (32 + nodeOfPointer->size);
        bwdNode->fwd = nodeOfPointer->fwd;
    }

    if (!nodeOfPointer->is_free) {
        nodeOfPointer->is_free = FREE;
    }
}

