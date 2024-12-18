#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>
#include "utils.h"

int main(void) {
    int id = 0, err = 0;
    SharedMem memory;

    id = initSharedMemory(100);

    memory = (SharedMem) shmat(id, (void*)0, 0);
    if (*(int*)memory == -1) printf("error in attachment\n");
    else printf("attachment succesfull\n");

    printf("start monitor\n"); getchar();

    destroySharedMemory(memory, id);
    err = shmdt((void*) memory);
    if (err == -1) perror("Detachment\n");
    else printf("succesfully detached\n");
}