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
void* _arena_start;
node_t primaryNode = NULL;

int statusno = ERR_UNINITIALIZED;


extern int init(size_t size) {
    printf("   requested size %d bytes\n", size);
    if (size > MAX_ARENA_SIZE) {
        printf("   error: requested size larger than MAX_ARENA_SIZE (%d)\n", MAX_ARENA_SIZE);
        return ERR_BAD_ARGUMENTS;
    } else if (size < 1) {
        printf("   error: requested size is negative\n");
        return ERR_BAD_ARGUMENTS;
    }
    int pageSize = getpagesize();
    printf("   pageSize is %d bytes\n", pageSize);
    totalMemorySize = pageSize*size;
    printf("   adjusted size is %d bytes\n", totalMemorySize);
    int fd = open("/dev/zero",O_RDWR);
    _arena_start = mmap(NULL, totalMemorySize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    printf("   arena starts at %p\n", &_arena_start);
    printf("   arena ends at %p\n", (&_arena_start+totalMemorySize));

    printf("   initializing header for initial free chunk\n");
    primaryNode.size = totalMemorySize-sizeof(node_t);
    primaryNode.bwd = NULL;
    primaryNode.fwd = NULL;
    primaryNode.is_free = FREE;
    printf("   header size is %lu bytes\n", sizeof(node_t));
    return 1;
}

extern int destroy() {
    printf("   unmapping arena with munmap()\n");
    munmap(_arena_start, totalMemorySize);
    totalMemorySize = -1;
    statusno = ERR_UNINITIALIZED;
    primaryNode = NULL;
}

extern void* walloc(size_t size){

}

extern void wfree(void *ptr){

}

