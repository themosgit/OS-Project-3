#include "../include/utils.h"
#include <stdio.h>

int main(void) {
    int id = 0, err = 0;
    SharedMem memory;

    id = initSharedMemory();

    memory = (SharedMem) shmat(id, (void*)0, 0);
    if (*(int*)memory == -1) printf("error in attachment\n");
    else printf("Attachment succesfull\n");

    printf("Start receptionist execution\n");

    char ch;
    do{
        printf("Press q to stop receptionist\n");
        ch = getchar();
    }while(ch != 'q');
    printf("Please wait for all visitors in tables to finish\n");
    waitForReceptionist(memory);
    printf("Receptionist cleared queue and cleaned\n");
    destroySharedMemory(memory, id);
}