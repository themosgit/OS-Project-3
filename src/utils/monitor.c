#include "../include/utils.h"
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    assert(argc == 3);
    assert(argv[1][1] == 's');
    int id = atoi(argv[2]);
    SharedMem data = (SharedMem) shmat(id, (void*) 0, 0);
    if (*(int*)data == -1){
    perror("error in attachment\n");
    shmctl(id, IPC_RMID, NULL);
  }
    assert(data);
    printf("shared Mem attached\n Press q to stop\n");
    do{
    printf("BUFF SIZE:%ld\n", circularBuffSize(data));
    printf("BUFF Capacity:%d\n", circularBuffCapacity(data));
    printf("Buffer full flag is %d \n", circularBuffFull(data));
    printf("buffer empty flag is : %d\n", circularBuffEmpty(data));
    printf("buffer mutex is: %d\n", circularBuffMutexVal(data));
    printf("buffer head is:%d tail is:%d\n", circularBuffHeadVal(data), circularBuffTailVal(data));

    }while(getchar() != 'q');
    
    int  err = shmdt((void*) data);
    if (err == -1) perror("Detachment\n");

}