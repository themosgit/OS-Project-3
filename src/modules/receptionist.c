#include "../include/utils.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
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
            shmid = atoi(argv[4]);
            orderTime = atoi(argv[2]);
            break;
        default:
            printf("error in command syntax...\n");
            return 0;
    }

    SharedMem SharedMemory;
    SharedMemory = (SharedMem) shmat(shmid, (void*) 0, 0);
    assert(*(int*)SharedMemory != -1);
    assert(SharedMemory);

    while(!isBarClosed(SharedMemory)) {
        if (!circularBuffEmpty(SharedMemory) && seatAvailable(SharedMemory)) {
            circularBuffTail(SharedMemory);
            waitForVisitor(SharedMemory);
            //sleep(rand{torder})
        } else {
            clearTables(SharedMemory);
        }
    } 
    assert(shmdt((void*) SharedMemory) != -1);
    return 0;
}
