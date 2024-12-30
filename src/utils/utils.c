#include "../include/utils.h"
#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

struct sharedmem {
  sem_t buffer[100];
  size_t head;
  size_t tail;
  size_t capacity;
  bool full;
  sem_t mutex;

  bool stop;
  int seats[12];
  sem_t seatMutex;
  sem_t findSeat;

  sem_t statsMutex;
  int NumOfWaters;
  int NumOfCheeses;
  int NumOfWines;
  int NumOfSalads;
  int VisitDuration;
  int NumberOfVisitors;
  int WaitingTime;

  sem_t done;
};

int initSharedMemory() {
  size_t circularBufferSize = 100;
  size_t sharedMemSize = sizeof(struct sharedmem);
  int id = shmget(IPC_PRIVATE, sharedMemSize, 0666);
  if (id == -1) perror("Creation of shared memory failed..\n");

  printf("\nSharedMem on: %d\n", id);
  SharedMem memory = (SharedMem) shmat(id, NULL, 0);
  if (*(int*)memory == -1){
    perror("error in attachment\n");
    shmctl(id, IPC_RMID, NULL);
  }


  memory->capacity = circularBufferSize;
  memory->head = 0;
  memory->tail = 0;
  memory->full = false;
  sem_init(&memory->mutex, 1, 1);
  sem_init(&memory->seatMutex, 1, 1);
  sem_init(&memory->findSeat, 1, 1);
  memory->stop = false;
  for (size_t i = 0; i < 12; i++) {
    memory->seats[i] = 0;
  }

  sem_init(&memory->statsMutex,1, 1);
  memory->NumOfWaters = 0;
  memory->NumOfCheeses = 0;
  memory->NumOfWines = 0;
  memory->NumOfSalads = 0;
  memory->VisitDuration = 0;
  memory->NumberOfVisitors = 0;
  memory->WaitingTime = 0;

  sem_init(&memory->done,1 , 0);

  for (size_t i = 0; i < circularBufferSize; ++i) {
    sem_init(&memory->buffer[i], 1, 0);
  }

  return id;
}


void destroySharedMemory(SharedMem memory, int shmid) {
  for(size_t i = 0; i < memory->capacity; ++i) {
    sem_destroy(&memory->buffer[i]);
  }
  sem_destroy(&memory->mutex);
  sem_destroy(&memory->seatMutex);
  sem_destroy(&memory->findSeat);
  sem_destroy(&memory->seatMutex);
  sem_destroy(&memory->done);

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
  return (!memory->full && (memory->tail == memory->head));
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
  int pos, success = 0;
  assert(memory && memory->buffer);
  if(!memory->full) {
    sem_wait(&memory->mutex);
    advanceHead(memory);
    pos = memory->head - 1;
    if(pos == -1) pos = memory->capacity - 1;
    sem_post(&memory->mutex);
    sem_wait(&memory->buffer[pos]);
    success = 1;
  }
  return success;
}

int circularBuffTail(SharedMem memory) {
  assert(memory && memory->buffer);
  int success = 0;
  if(!circularBuffEmpty(memory)) {
    sem_wait(&memory->mutex);
    sem_post(&memory->buffer[memory->tail]);
    retreatTail(memory);
    sem_post(&memory->mutex);
    success = 1;
  }
  return success;
}


bool BarClosed(SharedMem memory) {
  return memory->stop;
}

bool seatAvailable(SharedMem memory) { 
  for (size_t i = 0; i < 12; i++) {
    if(memory->seats[i] == 0) return true;
  }
  return false;
}

void waitForVisitor(SharedMem memory) {
  sem_wait(&memory->findSeat);
}

int findSeatIndex(SharedMem memory) {
  sem_wait(&memory->seatMutex);
  for (size_t i = 0; i < 12; i++) {
    if(memory->seats[i] == 0) {
      memory->seats[i] = getpid();
      sem_post(&memory->seatMutex);
      return i;
    }
  }
  return -1;
}

void markSeatDirty(SharedMem memory, int index) {
  sem_wait(&memory->seatMutex);
  memory->seats[index] = -1;
  sem_post(&memory->seatMutex);
}

void clearTables(SharedMem memory) {
  sem_wait(&memory->seatMutex);
  int j = 0;
  while(j < 12){
    if (memory->seats[j] == memory->seats[j+1] &&
        memory->seats[j+1] == memory->seats[j+2] &&
        memory->seats[j+2] == memory->seats[j+3]) {
          memory->seats[j] = 0;
          memory->seats[j+1] = 0;
          memory->seats[j+2] = 0;
          memory->seats[j+3] = 0;
    }
    j += 4;      
  }
  if (BarClosed(memory)) {
    for (size_t i = 0; i < 12; i++) {
      if (memory->seats[i] == -1) {
        memory->seats[i] = 0;
      }
    }
  }
  
  sem_post(&memory->seatMutex);
}

void updateSharedMemStats(SharedMem memory, int drink, int food, int visitDuration, int waitTime) {
  sem_wait(&memory->statsMutex);
  memory->NumberOfVisitors++;
  memory->VisitDuration += visitDuration;
  memory->WaitingTime +=waitTime;
  switch (drink) {
  case 0:
    memory->NumOfWaters++;
    break;
  case 1:
    memory->NumOfWines++;
    break;
  case 2:
    memory->NumOfWaters++;
    memory->NumOfWines++;
    break;
  }

  switch (food) {
  case 0:
    memory->NumOfCheeses++;
    break;
  case 1:
    memory->NumOfSalads++;
    break;
  }
  sem_post(&memory->statsMutex);
}

void printStats(SharedMem memory) {
  sem_wait(&memory->statsMutex);
  printf("Number of visitors: %d\n", memory->NumberOfVisitors);
  printf("Total wait time: %d\n", memory->WaitingTime);
  printf("Total visit duration: %d\n", memory->VisitDuration);
  printf("Number of waters consumed: %d\n", memory->NumOfWaters);
  printf("Number of wines consumed: %d\n", memory->NumOfWines);
  printf("Number of cheese platters consumed: %d\n", memory->NumOfCheeses);
  printf("Number of salads consumed: %d\n", memory->NumOfSalads);
  sem_post(&memory->statsMutex);
}

void tableState(SharedMem memory) {
  sem_wait(&memory->seatMutex);
  for (size_t i = 0; i < 12; i++) {
    if (i % 4 == 0 && i != 0) printf("\n");
    printf("[%d]", memory->seats[i]);
  }
  sem_post(&memory->seatMutex);
}

bool BarEmpty(SharedMem memory){
 int i = 0;
 while (memory->seats[i] == 0 && i < 12) {
  i++;
 }
 if (i == 12) return true;
 else return false;
}

void waitForReceptionist(SharedMem memory) {
  memory->stop = true;
  sem_wait(&memory->done);
} 

void receptionistDone(SharedMem memory) {
  sem_post(&memory->done);
}

int circularBuffMutexVal(SharedMem memory) {
  return sem_trywait(&memory->mutex);
}

int circularBuffHeadVal(SharedMem memory) {
  return memory->head;
}

int circularBuffTailVal(SharedMem memory) {
  return memory->tail;
}