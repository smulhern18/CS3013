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
        printf("   error: requested size is negative or zero\n");
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
    node_t* previousNode = NULL;
    int bytesMoved = 0;
    short hasSpace = 0;

    while(hasSpace == 0) {
        if (currentNode->is_free == FREE) {
            printf("   found a free chunk of %d bytes with a header at %p\n", (int) currentNode->size, incrimentableArenaStart+bytesMoved);
            if (currentNode->size > (size+sizeof(node_t))) {
                size_t freeSpace = currentNode->size - sizeof(node_t) - size;
                currentNode->size = size;
                currentNode->is_free = TAKEN;
                node_t* nextNode;
                nextNode = (node_t*) (((char*)currentNode) + size + sizeof(node_t));
                nextNode->size = freeSpace;
                nextNode->is_free = FREE;
                nextNode->bwd = (struct __node_t*) currentNode;
                nextNode->fwd = NULL;
                currentNode->fwd = (struct __node_t*) nextNode;
                currentNode->bwd = (struct __node_t*) previousNode;

            } else if ((currentNode->size)<(size)) {
                statusno = ERR_OUT_OF_MEMORY;
                return NULL;
            } else {
                currentNode->is_free = TAKEN;
            }
            bytesMoved+=sizeof(node_t);
            hasSpace = 1;
        } else {
            bytesMoved += (sizeof(node_t) + currentNode->size);
            if (bytesMoved >= totalMemorySize){
                break;
            }
            previousNode = currentNode;
            currentNode = currentNode->fwd;
        }
    }

    if (hasSpace == 0) {
        statusno = ERR_OUT_OF_MEMORY;
        return NULL;
    }
    incrimentableArenaStart = ((char*)currentNode)+sizeof(node_t);
    printf("   %p is the location of the allocated chunk!\n", incrimentableArenaStart);
    return (void*) incrimentableArenaStart;

}

extern void wfree(void *ptr){
    if (ptr == NULL) {
        statusno = ERR_BAD_ARGUMENTS;
    }
    printf("   supplied pointer %p\n", ptr);
    node_t* nodeOfPointer = (node_t*) (((char*) ptr) - sizeof(node_t));
    printf("   accessing chunk header at %p\n", nodeOfPointer);
    nodeOfPointer->is_free = FREE;
    while (nodeOfPointer->fwd != NULL && nodeOfPointer->fwd->is_free) {
        printf("   Adding forward Node to the current node!\n");
        node_t* fwdNode = nodeOfPointer->fwd;
        nodeOfPointer->size += (sizeof(node_t) + fwdNode->size);
        nodeOfPointer->fwd = fwdNode->fwd;
        fwdNode->bwd = NULL;
        fwdNode->fwd = NULL;
    }
    while (nodeOfPointer->bwd != NULL && nodeOfPointer->bwd->is_free) {
        printf("   Adding current node to the backwards node!\n");
        node_t *bwdNode = nodeOfPointer->bwd;
        bwdNode->size += (sizeof(node_t) + nodeOfPointer->size);
        if (bwdNode->size > (totalMemorySize-sizeof(node_t))) {
            bwdNode->size = totalMemorySize-sizeof(node_t);
        }
        bwdNode->fwd = nodeOfPointer->fwd;
        nodeOfPointer = bwdNode;
    }
}

