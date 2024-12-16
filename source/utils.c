#include "utils.h"
#include <stdlib.h>
#include <semaphore.h>
#include <assert.h>

struct sharedmem {

  CircularBuff buffer;

  int NumOfWaters;
  int NumOfCheeses;
  int NumOfWines;
  int NumOfSalads;
  int AvgVisitDuration;
  int NumberOfVisitors;
  int AvgWaitingTime;
};

struct circularbuff{
  sem_t *buffer;
  size_t head;
  size_t tail;
  size_t size;//size here is basically capacity
  sem_t mutex;
}; 

CircularBuff circularBuffInit(size_t size) {
  CircularBuff circBuff = malloc(sizeof(struct circularbuff));
  assert(circBuff);
  circBuff->buffer = malloc(sizeof(sem_t) * size);
  for(size_t i = 0; i < size; i++) {
    if(sem_init(&circBuff->buffer[i], 1, 0));
  }
  sem_init(&circBuff->mutex, 1, 1);
  circBuff->size = size;
  circBuff->head = 0;
  circBuff->tail = 0;
  assert(circularBuffEmpty(circBuff));
  return circBuff;
}

void circularBuffDestroy(CircularBuff circBuff) {
  assert(circBuff);
  assert(circBuff->buffer);
  for(size_t i = 0; i < circBuff->size; i++) sem_destroy(&circBuff->buffer[i]);
  sem_destroy(&circBuff->mutex);
  free(circBuff->buffer);
  free(circBuff);
}

int circularBuffCapacity(CircularBuff circBuff) {
  assert(circBuff);
  size_t size = circBuff->size;
}

size_t circularBuffSize(CircularBuff circBuff) {
  assert(circBuff);
  size_t size = circBuff->size;
  if()
}


int circularBuffFull(CircularBuff circBuff) {
  size_t head = circBuff->head + 1;
  if (head == circBuff->size) head = 0;
  return head == circBuff->tail;
}

int circularBuffEmpty(CircularBuff circBuff) {
  return circBuff->head == circBuff->tail;
}

int circularBuffTail(CircularBuff circBuff) {
  int success = 0;
  if(circBuff && !circularBuffEmpty(circBuff)) {
    circBuff->tail = (circBuff->tail + 1) % circBuff->size
;
    success = 1;
  }
  return success;
}

int circularBuffHead(CircularBuff circBuff) {
  int success = 0;
  if(circBuff && !circularBuffFull(circBuff)) {
    circBuff->head = (circBuff->head + 1) % circBuff->size;
    success = 1;
  }
  return success;
}

