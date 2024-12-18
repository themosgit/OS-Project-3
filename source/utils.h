#pragma once
#include <stddef.h>
#include <stdbool.h>

typedef struct sharedmem* SharedMem;
typedef struct circularbuff* CircularBuff;

typedef void* Pointer; 

void circularBuffInit(CircularBuff buffer,size_t size);

void circularBuffReset(CircularBuff circBuff);

void circularBuffDestroy(CircularBuff circBuff);

int circularBuffCapacity(CircularBuff circBuff);

size_t circularBuffSize(CircularBuff circBuff);

bool circularBuffFull(CircularBuff circBuff);

bool circularBuffEmpty(CircularBuff circBuff);

static void advanceHead(CircularBuff circBuff);

static void retreatTail(CircularBuff circBuff);

int circularBuffHead(CircularBuff circBuff);

int circularBuffTail(CircularBuff circBuff);



int initSharedMemory(size_t circularBufferSize);

void destroySharedMemory(SharedMem mem, int shmid);

size_t getSize();

int buffsize(SharedMem mem);

int buffCapacity(SharedMem mem);


