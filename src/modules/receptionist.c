#include "../include/utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

int main(int argc, char *argv[]) {
    //Checks for correct argument syntax
    assert(argc == 5);
    int orderTime, shmid;
    switch (argv[1][1]) {
        case 'd':
            assert(argv[3][1] == 's');
            orderTime = atoi(argv[2]);
            shmid = atoi(argv[4]);
            break;
        case 's':
            assert(argv[3][1] == 'd');
            shmid = atoi(argv[2]);
            orderTime = atoi(argv[4]);
            break;
        default:
            printf("error in command syntax...\n");
            return 0;
    }

    //attaches pre-initialized shared memory segment based on shmid
    SharedMem SharedMemory;
    SharedMemory = (SharedMem) shmat(shmid, (void*) 0, 0);
    assert(*(int*)SharedMemory != -1);
    assert(SharedMemory);

    //starts logging on Receptionistlog.txt
    Receptionistlog_init(SharedMemory);

    //calculates time range
    int randOrderTime, minOrderTime = ceil(0.5*orderTime);
    srand(time(NULL));

    //main receptionist loop
    while(!BarClosed(SharedMemory) || !BarEmpty(SharedMemory)) {
        if (!circularBuffEmpty(SharedMemory) && seatAvailable(SharedMemory) && !BarClosed(SharedMemory)) {
            circularBuffTail(SharedMemory);//wakes waiting visitor
            waitForVisitor(SharedMemory);
            randOrderTime = rand() % (orderTime - minOrderTime + 1)  + minOrderTime;
            sleep(randOrderTime);
        } else {
            clearTables(SharedMemory);
        }
    } 
    //clears visitors in queue (all visitors are detached)
    while (!circularBuffEmpty(SharedMemory)) {
        circularBuffTail(SharedMemory);
    }

    printStats(SharedMemory);
    //posts driver process to proceed destruction of shared memory
    receptionistDone(SharedMemory);
    //closes Receptionistlog.txt
    Receptionistlog_close(SharedMemory);
    
    assert(shmdt((void*) SharedMemory) != -1);
    return 0;
}
