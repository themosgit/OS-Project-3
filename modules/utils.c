#include "../include/utils.h"
#include <stdlib.h>
#include <semaphore.h>
#include <assert.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>

struct sharedmem {

  sem_t *buffer;
  size_t head;
  size_t tail;
  size_t capacity;
  bool full;

  int NumOfWaters;
  int NumOfCheeses;
  int NumOfWines;
  int NumOfSalads;
  int AvgVisitDuration;
  int NumberOfVisitors;
  int AvgWaitingTime;
};

int initSharedMemory(size_t circularBufferSize) {

  size_t sharedMemSize = sizeof(struct sharedmem) + sizeof(sem_t) * circularBufferSize;

  int id = shmget(IPC_PRIVATE, sharedMemSize, 0666);
  if (id == -1) perror("Creation of shared memory failed..\n");

  SharedMem memory = (SharedMem) shmat(id, NULL, 0);
  if (*(int*)memory == -1){
    perror("error in attachment\n");
    shmctl(id, IPC_RMID, NULL);
  }

  memory->buffer = (sem_t*)((char*)memory + sizeof(struct sharedmem));

  for (size_t i = 0; i < circularBufferSize; ++i) {
    sem_init(&memory->buffer[i], 1, 0);
  }
  memory->capacity = circularBufferSize;
  memory->head = 0;
  memory->tail = 0;
  memory->full = false;
  
  memory->NumOfWaters = 0;
  memory->NumOfCheeses = 0;
  memory->NumOfWines = 0;
  memory->NumOfSalads = 0;
  memory->AvgVisitDuration = 0;
  memory->NumberOfVisitors = 0;
  memory->AvgWaitingTime = 0;

  return id;
}


void destroySharedMemory(SharedMem memory, int shmid) {
  for(size_t i = 0; i < memory->capacity; ++i) {
    sem_destroy(&memory->buffer[i]);
  }
  if(shmdt(memory) == -1) perror("shmdt failed\n");
  if(shmctl(shmid, IPC_RMID, NULL) == -1) perror("shmctl failed\n");
}

int circularBuffCapacity(SharedMem memory) {
  assert(memory->buffer);
  return memory->capacity;
}

size_t circularBuffSize(SharedMem memory) {
  assert(memory->buffer);
  size_t size;
  if(memory->head >= memory->tail) {
    size = memory->head - memory->tail;
  } else {
    size = memory->capacity + memory->head - memory->tail;
  }
  if(memory->full) size = memory->capacity;
  return size;
}


bool circularBuffFull(SharedMem memory) {
  assert(memory->buffer);
  return memory->full;
}

bool circularBuffEmpty(SharedMem memory) {
  assert(memory->buffer);
  return (!memory->full &&(memory->tail == memory->head));
}

static void advanceHead(SharedMem memory) {
  assert(memory->buffer);
  if (memory->full) {
    if(++memory->tail == memory->capacity) {
      memory->tail = 0;
    }
  }
  if (++memory->head == memory->capacity) {
    memory->head = 0;
  }
  memory->full = memory->head == memory->tail;
}

static void retreatTail(SharedMem memory) {
  assert(memory->buffer);
  memory->full = false;
  if (++(memory->tail) == memory->capacity) {
    memory->tail = 0;
  }
}

int circularBuffHead(SharedMem memory) {
  int success = 0;
  assert(memory && memory->buffer);
  if(!memory->full) {
    advanceHead(memory);
    success = 1;
  }
  return success;
}

int circularBuffTail(SharedMem memory) {
  int success = 0;
  assert(memory && memory->buffer);
  if(!circularBuffEmpty(memory)) {
    retreatTail(memory);
    success = 1;
  }
  return success;
}
