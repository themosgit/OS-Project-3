#pragma once
#include <stddef.h>
#include <stdbool.h>

typedef struct sharedmem* SharedMem;

typedef void* Pointer; 


int initSharedMemory(size_t circularBufferSize);

void destroySharedMemory(SharedMem mem, int shmid);

int circularBuffCapacity(SharedMem memory);

size_t circularBuffSize(SharedMem memory);

bool circularBuffFull(SharedMem memory);

bool circularBuffEmpty(SharedMem memory);

static void advanceHead(SharedMem memory);

static void retreatTail(SharedMem memory);

int circularBuffHead(SharedMem memory);

int circularBuffTail(SharedMem memory);

int circularBuffMutexVal(SharedMem memory);

int circularBuffHeadVal(SharedMem memory);

int circularBuffTailVal(SharedMem memory);