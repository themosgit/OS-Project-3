#include "../include/utils.h"
#include "../include/acutest.h"
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


void test_init(void) {
    int shmid = initSharedMemory(1000);
    SharedMem memory = (SharedMem) shmat(shmid, NULL, 0);
    
    TEST_ASSERT(memory != NULL);
    TEST_ASSERT(circularBuffCapacity(memory) == 1000);
    TEST_ASSERT(circularBuffSize(memory) == 0);
    TEST_ASSERT(circularBuffEmpty(memory));

    destroySharedMemory(memory, shmid);
    TEST_ASSERT(shmdt((void*)memory));

}
void test_insert_remove(void) {
    int buff_size = 10000;
    int shmid = initSharedMemory(buff_size);
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

TEST_LIST = {
    {"Circular Buffer Intialiazation", test_init},
    {"Circular Buffer Insert/Remove", test_insert_remove},
    {NULL, NULL}
};

