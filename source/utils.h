#pragma once
#include <stddef.h>

typedef struct sharedmem* SharedMem;
typedef struct circularbuff* CircularBuff;

typedef void* Pointer;

CircularBuff circularBuffInit(size_t size);

void circularBuffDestroy(CircularBuff circBuff);

int circularBuffCapacity(CircularBuff circBuff);

int circularBuffFull(CircularBuff circBuff);

int circularBuffEmpty(CircularBuff circBuff);

int circularBuffTail(CircularBuff circBuff);

int circularBuffHead(CircularBuff circBuff);