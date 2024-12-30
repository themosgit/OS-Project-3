#include "../include/utils.h"
#include <stdio.h>

/*driver.c creates the shared memory segment 
* after the creation receptionist and visitors
* shall be started with the output shmid.
* Waits for q from user input to signal stop to 
* the receptionist then waits for the remaining jobs 
* to complete and clears the shared memory segment.
*/

int main(void) {
    int id = 0, err = 0;
    SharedMem memory;

    id = initSharedMemory(); // creates shared memory

    memory = (SharedMem) shmat(id, (void*)0, 0);
    if (*(int*)memory == -1) printf("error in attachment\n");
    else printf("Attachment succesfull\n");

    printf("Start receptionist and visitor execution\n");

    char ch;
    do{
        printf("Press q to stop\n");
        ch = getchar();
    }while(ch != 'q');

    printf("Please wait for all visitors in tables to finish\n");
    waitForReceptionist(memory); // signals stop and waits
    printf("Receptionist cleared queue and cleaned\n");
    destroySharedMemory(memory, id); // destroys shared memory
}