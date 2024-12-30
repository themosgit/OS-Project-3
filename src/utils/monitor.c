#include "../include/utils.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    assert(argc == 3);
    assert(argv[1][1] == 's');
    int id = atoi(argv[2]);
    SharedMem data = (SharedMem) shmat(id, (void*) 0, 0);
    if (*(int*)data == -1){
    perror("error in attachment\n");
  }
    assert(data);
    printf("shared Mem attached\n Press q to stop\n");
    do{
      printf("Circular Buffer Size: %ld\n", circularBuffSize(data));
      printStats(data);
      tableState(data);
    }while(getchar() != 'q');
    
    int  err = shmdt((void*) data);
    if (err == -1) perror("Detachment\n");

}