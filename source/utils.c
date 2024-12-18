#include "utils.h"
#include <stdlib.h>
#include <semaphore.h>
#include <assert.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>



struct circularbuff{
  sem_t *buffer;
  size_t head;
  size_t tail;
  size_t capacity;// capacity here is basically capacity
  sem_t mutex;
  bool full;
}; 

struct sharedmem {

  struct circularbuff buffer;

  int NumOfWaters;
  int NumOfCheeses;
  int NumOfWines;
  int NumOfSalads;
  int AvgVisitDuration;
  int NumberOfVisitors;
  int AvgWaitingTime;
};

void circularBuffInit(CircularBuff buffer, size_t capacity) {
  buffer->buffer = (sem_t*)((char*)buffer + sizeof(struct circularbuff));
  for(size_t i = 0; i < capacity; i++) {
    if(sem_init(&buffer->buffer[i], 1, 0));
  }
  sem_init(&buffer->mutex, 1, 1);
  buffer->capacity = capacity;
  circularBuffReset(buffer);
  assert(circularBuffEmpty(buffer));
}

void circularBuffReset(CircularBuff circBuff) {
  assert(circBuff);
  circBuff->head = 0;
  circBuff->tail = 0;
  circBuff->full = false;
}

void circularBuffDestroy(CircularBuff circBuff) {
  assert(circBuff);
  assert(circBuff->buffer);
  for(size_t i = 0; i < circBuff->capacity; i++) sem_destroy(&circBuff->buffer[i]);
  sem_destroy(&circBuff->mutex); 
}

int circularBuffCapacity(CircularBuff circBuff) {
  assert(circBuff);
  size_t capacity = circBuff->capacity;
  return capacity;
}

size_t circularBuffSize(CircularBuff circBuff) {
  assert(circBuff);
  size_t size;
  if(circBuff->head >= circBuff->tail) {
    size = circBuff->head - circBuff->tail;
  } else {
    size = circBuff->capacity + circBuff->head - circBuff->tail;
  }
  if(circBuff->full) size = circBuff->capacity;
  return size;
}


bool circularBuffFull(CircularBuff circBuff) {
  assert(circBuff);
  return circBuff->full;
}

bool circularBuffEmpty(CircularBuff circBuff) {
  assert(circBuff);
  return (!circBuff->full &&(circBuff->tail == circBuff->head));
}

static void advanceHead(CircularBuff circBuff) {
  assert(circBuff);
  if (circBuff->full) {
    if(++circBuff->tail == circBuff->capacity) {
      circBuff->tail = 0;
    }
  }
  if (++circBuff->head == circBuff->capacity) {
    circBuff->head = 0;
  }
  circBuff->full = circBuff->head == circBuff->tail;
}

static void retreatTail(CircularBuff circBuff) {
  assert(circBuff);
  circBuff->full = false;
  if (++(circBuff->tail) == circBuff->capacity) {
    circBuff->tail = 0;
  }
}

int circularBuffHead(CircularBuff circBuff) {
  int success = 0;
  assert(circBuff && circBuff->buffer);
  if(!circularBuffFull(circBuff)) {
    advanceHead(circBuff);
    success = 1;
  }
  return success;
}

int circularBuffTail(CircularBuff circBuff) {
  int success = 0;
  assert(circBuff && circBuff->buffer);
  if(!circularBuffEmpty(circBuff)) {
    retreatTail(circBuff);
    success = 1;
  }
  return success;
}

int initSharedMemory(size_t circularBufferSize) {

  size_t sharedMemSize = sizeof(struct sharedmem);
  sharedMemSize +=  sizeof(sem_t) * circularBufferSize;

  int id = shmget(IPC_PRIVATE, sharedMemSize, 0666);
  if (id == -1) perror("Creation of shared memory failed..\n");
  else printf("share memory segment created with id %d\n", id);
  SharedMem memory = (SharedMem) shmat(id, (void*)0, 0);
  if (*(int*)memory == -1) printf("error in attachment\n");
  else printf("attachment succesfull\n");
 
  circularBuffInit(&memory->buffer, circularBufferSize);
  memory->NumOfWaters = 0;
  memory->NumOfCheeses = 0;
  memory->NumOfWines = 0;
  memory->NumOfSalads = 0;
  memory->AvgVisitDuration = 0;
  memory->NumberOfVisitors = 0;
  memory->AvgWaitingTime = 0;

  return id;
}

void destroySharedMemory(SharedMem mem, int shmid) {
  circularBuffDestroy(&mem->buffer);
  if (shmdt(mem) == -1) perror("shmdt");
  if (shmctl(shmid, IPC_RMID, NULL) == -1) perror("shmctl");
}

size_t getSize(){
  return sizeof(SharedMem);
}

int buffsize(SharedMem mem) {
  return circularBuffSize(&mem->buffer);
}

int buffCapacity(SharedMem mem) {
  return circularBuffCapacity(&mem->buffer);
}