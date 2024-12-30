#pragma once
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include "log.h"

typedef struct sharedmem* SharedMem;

int initSharedMemory();

void destroySharedMemory(SharedMem mem, int shmid);

int circularBuffCapacity(SharedMem memory);

size_t circularBuffSize(SharedMem memory);

bool circularBuffFull(SharedMem memory);

bool circularBuffEmpty(SharedMem memory);

static void advanceHead(SharedMem memory);

static void retreatTail(SharedMem memory);

int circularBuffHead(SharedMem memory);

int circularBuffTail(SharedMem memory);



bool BarClosed(SharedMem memory);

bool seatAvailable(SharedMem memory);

void waitForVisitor(SharedMem memory);

int findSeatIndex(SharedMem memory);

void markSeatDirty(SharedMem memory, int index);

void clearTables(SharedMem memory);

void updateSharedMemStats(SharedMem memory, int drink, int food, int visitDuration, int waitTime);

void printStats(SharedMem memory);

void tableState(SharedMem memory);

bool BarEmpty(SharedMem memory);

void waitForReceptionist(SharedMem memory);

void receptionistDone(SharedMem memory);

int circularBuffMutexVal(SharedMem memory);

int circularBuffHeadVal(SharedMem memory);

int circularBuffTailVal(SharedMem memory);

void log_init(SharedMem memory);

void Receptionistlog_init(SharedMem memory);

void Receptionistlog_close(SharedMem memory);
