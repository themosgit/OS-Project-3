#include "../include/utils.h"
#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
/*utils.c implements all necessary functions 
* structures used by driver.c receptionist.c
* visitor.c and monitor.c for an explanation
* of the expected actions of each function 
* please read utils.h Note: every semaphore 
* with mutex in its name is initialized with 1
* while the others with 0
*/

struct sharedmem {
  //circular buffer
  sem_t buffer[100]; //semaphores for visitors to wait
  size_t head;
  size_t tail;
  size_t capacity;
  bool full; //flag for circular buffer full
  sem_t mutex; //mutex for circular buffer

  bool stop; //flag for clearing bar

  int seats[12];
  sem_t seatMutex;//mutex for seat assignemt
  sem_t findSeat;//semaphore for receptionist to wait

  sem_t statsMutex;
  int NumOfWaters;
  int NumOfCheeses;
  int NumOfWines;
  int NumOfSalads;
  int VisitDuration;
  int NumberOfVisitors;
  int WaitingTime;

  sem_t done;//samaphore to signal receptionist completion

  //log files
  FILE* Driverlog; 
  FILE* Receptionistlog;
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
  
  for (size_t i = 0; i < circularBufferSize; ++i) {
    sem_init(&memory->buffer[i], 1, 0);
  }

  sem_init(&memory->mutex, 1, 1);
  sem_init(&memory->seatMutex, 1, 1);
  sem_init(&memory->findSeat, 1, 1);
  sem_init(&memory->statsMutex,1, 1);
  sem_init(&memory->done,1 , 0);

  memory->stop = false;
  for (size_t i = 0; i < 12; i++) {
    memory->seats[i] = 0;
  }

  memory->NumOfWaters = 0;
  memory->NumOfCheeses = 0;
  memory->NumOfWines = 0;
  memory->NumOfSalads = 0;
  memory->VisitDuration = 0;
  memory->NumberOfVisitors = 0;
  memory->WaitingTime = 0;

  log_set_quiet(true);
  memory->Driverlog = fopen("Driverlog.txt", "w");
  log_add_fp(memory->Driverlog, 1);

  log_info("Shared merory segment created");

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

  log_info("Semaphores destroyed closing log...");
  fclose(memory->Driverlog);
  
  if(shmdt(memory) == -1) perror("shmdt failed\n");
  if(shmctl(shmid, IPC_RMID, NULL) == -1) perror("shmctl failed\n");
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
    log_info("Adding process on queue position: %d", pos);
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
    log_info("Removing process from queue position: %d", memory->tail);
    sem_post(&memory->buffer[memory->tail]);
    retreatTail(memory);
    sem_post(&memory->mutex);
    success = 1;
  }
  return success;
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



bool BarClosed(SharedMem memory) {
  return memory->stop;
}

bool seatAvailable(SharedMem memory) { 
  for (size_t i = 0; i < 12; i++) {
    if(memory->seats[i] == 0) return true;
  }
  return false;
}

bool BarEmpty(SharedMem memory){
 int i = 0;
 while (memory->seats[i] == 0 && i < 12) {
  i++;
 }
 if (i == 12) return true;
 else return false;
}

void clearTables(SharedMem memory) {
  sem_wait(&memory->seatMutex);
  int j = 0;
  while(j < 12){
    if (memory->seats[j] == memory->seats[j+1] &&
        memory->seats[j+1] == memory->seats[j+2] &&
        memory->seats[j+2] == memory->seats[j+3] &&
        memory->seats[j] == -1) {
          log_info("Table %d cleaned", j);
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
        log_info("Bar is closed cleaning seat %d", i);
        memory->seats[i] = 0;
      }
    }
  }
  sem_post(&memory->seatMutex);
}


void receptionistDone(SharedMem memory) {
  log_info("Receptionist done ending execution");
  sem_post(&memory->done);
}

void waitForReceptionist(SharedMem memory) {
  memory->stop = true;
  log_info("Stop flag sent waiting for receptionist to complete");
  sem_wait(&memory->done);
} 

void waitForVisitor(SharedMem memory) {
  log_info("waiting for seat assignement");
  sem_wait(&memory->findSeat);
}


int findSeatIndex(SharedMem memory) {
  sem_wait(&memory->seatMutex);
  for (size_t i = 0; i < 12; i++) {
    if(memory->seats[i] == 0) {
      memory->seats[i] = getpid();
      log_info("Seat found at %d", i);
      sem_post(&memory->seatMutex);
      sem_post(&memory->findSeat);
      return i;
    }
  }
  return -1;
}

void markSeatDirty(SharedMem memory, int index) {
  sem_wait(&memory->seatMutex);
  log_info("Marking seat %d as dirty", index);
  memory->seats[index] = -1;
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
  default:
    break;
  }
  log_info("visitor updated stats and leaving");
  sem_post(&memory->statsMutex);
}

void printStats(SharedMem memory) {
  sem_wait(&memory->statsMutex);
  printf("Number of visitors: %d\n", memory->NumberOfVisitors);
  printf("Total wait time: %d Avarage: %d\n", memory->WaitingTime, memory->WaitingTime / memory->NumberOfVisitors);
  printf("Total visit duration: %d Avarage: %d\n", memory->VisitDuration, memory->VisitDuration / memory->NumberOfVisitors);
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

void Receptionistlog_init(SharedMem memory) {
  log_set_quiet(true);
  memory->Receptionistlog = fopen("Receptionistlog.txt", "w");
  log_add_fp(memory->Receptionistlog, 1);
  log_info("Receptionisist log started");
}

void Receptionistlog_close(SharedMem memory) {
  log_info("Closing receptionist log");
  fclose(memory->Receptionistlog);
}
