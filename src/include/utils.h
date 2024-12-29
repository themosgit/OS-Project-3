#pragma once
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct sharedmem* SharedMem;

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



bool isBarClosed(SharedMem memory);

bool seatAvailable(SharedMem memory);

void waitForVisitor(SharedMem memory);

int findSeatIndex(SharedMem memory);

void markSeatDirty(SharedMem memory, int index);

void clearTables(SharedMem memory);

int circularBuffMutexVal(SharedMem memory);

int circularBuffHeadVal(SharedMem memory);

int circularBuffTailVal(SharedMem memory);