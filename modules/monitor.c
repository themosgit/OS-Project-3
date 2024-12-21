#include "../include/utils.h"
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <assert.h>
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
    printf("shared Mem attached\n");

    printf("BUFF SIZE:%ld\n", circularBuffSize(data));
    printf("BUFF Capacity:%d\n", circularBuffCapacity(data));

}