#include "../include/utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>


int main(int argc, char *argv[]) {
    //Checks for correct argument syntax
    assert(argc == 5);
    int restTime, shmid;
    switch (argv[1][1]) {
        case 'd':
            assert(argv[3][1] == 's');
            restTime = atoi(argv[2]);
            shmid = atoi(argv[4]);
            break;
        case 's':
            assert(argv[3][1] == 'd');
            shmid = atoi(argv[2]);
            restTime = atoi(argv[4]);
            break;
        default:
            printf("error in command syntax...\n");
            return 0;
    }
    //if true no logging output set to false to view logs in tty
    log_set_quiet(true);
    //attaches pre-initialized shared memory segment based on shmid
    SharedMem SharedMemory;
    SharedMemory = (SharedMem) shmat(shmid, (void*) 0, 0);
    assert(*(int*)SharedMemory != -1);
    assert(SharedMemory);

    //starts waiting timer
    time_t start = time(NULL);
    //waits in queue
    circularBuffHead(SharedMemory);

    //if bar is closed detach and exit
    if(BarClosed(SharedMemory)){
        assert(shmdt((void*) SharedMemory) != -1);
        return 0;
    }

    //calculate waiting time
    time_t end = time(NULL);
    int waitTime = end - start;

    //find seat, order, rest, mark seat, update shared memory 
    int seatNum = findSeatIndex(SharedMemory);
    
    srand(time(NULL));
    int drink = rand() % 3;
    int food = rand() % 3;

    int randRestTime, minRestTime = ceil(0.7*restTime);
    randRestTime = rand() % (restTime - minRestTime + 1) + minRestTime;
    sleep(randRestTime);

    markSeatDirty(SharedMemory, seatNum);

    updateSharedMemStats(SharedMemory, drink, food, randRestTime, waitTime);
    
    assert(shmdt((void*) SharedMemory) != -1);
    return 0; 
}