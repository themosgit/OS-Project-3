#pragma once
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include "log.h"

typedef struct sharedmem* SharedMem; 

//function to create Shared Memory segment and semaphores returns shmid
int initSharedMemory();
//function to destroy Shared Memory segment and semaphores
void destroySharedMemory(SharedMem mem, int shmid);

//circular buffer utils
//advances head index checks for wrapping and sets full flag
static void advanceHead(SharedMem memory);
//advances tail index checks for wrapping
static void retreatTail(SharedMem memory);
//uses advanceHead() and halts process on semaphore
int circularBuffHead(SharedMem memory);
//uses retreatTail() and posts process on semaphore
int circularBuffTail(SharedMem memory);
//returns circular buffer capacity
int circularBuffCapacity(SharedMem memory);
//returns number of elements in buffer
size_t circularBuffSize(SharedMem memory);
//returns true if buffer is full
bool circularBuffFull(SharedMem memory);
//returns true if buffer is empty
bool circularBuffEmpty(SharedMem memory);

//receptionist utils
//returns stop flag value
bool BarClosed(SharedMem memory);
//returns true if any seat is 0
bool seatAvailable(SharedMem memory);
//returns true if all seats are 0
bool BarEmpty(SharedMem memory);
//clears tables in groups of 4
void clearTables(SharedMem memory);
//posts driver to clear memory
void receptionistDone(SharedMem memory);
//sets stop flag to true and waits for receptionistDone()
void waitForReceptionist(SharedMem memory);
//waits for visitor to find a seat
void waitForVisitor(SharedMem memory);

//visitor utils
//returns index of first seat where value is 0 using mutual exclusion
int findSeatIndex(SharedMem memory);
//marks seat of index -1
void markSeatDirty(SharedMem memory, int index);
//updates stats based on consumption and waiting time
void updateSharedMemStats(SharedMem memory, int drink, int food, int visitDuration, int waitTime);

//monitor/receptionist utils
//prints stats of shared memory with avarages
void printStats(SharedMem memory);
//displays all seat states
void tableState(SharedMem memory);

//Initialiazes logging for receptionist
void Receptionistlog_init(SharedMem memory);
//closes reseptionist log file
void Receptionistlog_close(SharedMem memory);
