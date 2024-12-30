#include "../include/utils.h"
#include "../include/acutest.h"
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

/*test_utils.c runs basic tests on the circular buffer 
* and proceeds to run a 30 second simulation of the bar
* on the path to nemea synchronization problem
*/

void test_init(void) {
    int shmid = initSharedMemory();
    SharedMem memory = (SharedMem) shmat(shmid, NULL, 0);
    
    TEST_ASSERT(memory != NULL);
    TEST_ASSERT(circularBuffCapacity(memory) == 100);
    TEST_ASSERT(circularBuffSize(memory) == 0);
    TEST_ASSERT(circularBuffEmpty(memory));

    destroySharedMemory(memory, shmid);
    TEST_ASSERT(shmdt((void*)memory));

}
void test_insert_remove(void) {
    int buff_size = 100;
    int shmid = initSharedMemory();
    SharedMem memory = (SharedMem) shmat(shmid, NULL, 0);

    TEST_ASSERT(memory != NULL);
    TEST_ASSERT(circularBuffCapacity(memory) == buff_size);
    int status;
    pid_t pid;
    for(int i = 0; i < buff_size; ++i) {
        if ((pid = fork()) == 0){
            TEST_ASSERT(circularBuffHead(memory));
            TEST_ASSERT(shmdt(memory) == 0);
            exit(0);
        }
    }
    
    while(circularBuffSize(memory) != buff_size);
    TEST_ASSERT(circularBuffHead(memory) == 0);
    TEST_ASSERT(circularBuffSize(memory) == buff_size);
    TEST_ASSERT(circularBuffFull(memory));
    
    for (int i = 0; i < 10; ++i) {
        TEST_ASSERT(circularBuffTail(memory));
        TEST_ASSERT(!circularBuffFull(memory));
        if ((pid = fork()) == 0){
            TEST_ASSERT(circularBuffHead(memory));
            TEST_ASSERT(shmdt(memory) == 0);
            exit(0);
        }
    }
    while(circularBuffSize(memory) != buff_size);
    
    for (int i = 0; i < buff_size; ++i) {
        TEST_ASSERT(circularBuffTail(memory));
    }

    wait(NULL);

    TEST_ASSERT(circularBuffEmpty(memory));
    destroySharedMemory(memory, shmid);
    TEST_ASSERT(shmdt(memory));
}

void test_path_to_nemea(void) {
    int shmid = initSharedMemory();

    SharedMem memory = (SharedMem) shmat(shmid, NULL, 0);
    TEST_ASSERT(memory != NULL);
    char argid[12];
    char argtime[2];
    sprintf(argid, "%d",shmid);
    sprintf(argtime, "%d",1);

    char *args[] = {"mparoufes","-s", argid, "-d", argtime, NULL};
    printf("Simulation will take 30 seconds to complete please wait\n");
    pid_t pid;
    if((pid = fork()) == 0) {
        execvp("receptionist", args);
    }
    for (int i = 0; i < 100; i++) { 
        if ((pid = fork()) == 0) {
            execvp("visitor", args);
        } 
    }
    sleep(30);
    waitForReceptionist(memory);

    TEST_ASSERT(circularBuffEmpty(memory));
    destroySharedMemory(memory, shmid);
    TEST_ASSERT(shmdt(memory));
}

TEST_LIST = {
    {"Circular Buffer Intialiazation", test_init},
    {"Circular Buffer Insert/Remove", test_insert_remove},
    {"Path To Nemea Simulation", test_path_to_nemea},
    {NULL, NULL}
};

