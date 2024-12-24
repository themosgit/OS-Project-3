#include "../include/utils.h"
#include "../include/acutest.h"
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


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
    int shmid = initSharedMemory(100);
    SharedMem memory = (SharedMem) shmat(shmid, NULL, 0);

    TEST_ASSERT(memory != NULL);
    TEST_ASSERT(circularBuffCapacity(memory) == 100);
    int status;
    pid_t pid;
    for(int i = 0; i < 100; ++i) {
        if ((pid = fork()) == 0){
            TEST_ASSERT(circularBuffHead(memory));
            int  err = shmdt((void*) memory);
            if (err == -1) perror("Detachment\n");
            exit(0);
        }
    }
    sleep(1);
    TEST_ASSERT(circularBuffHead(memory) == 0);
    TEST_ASSERT(circularBuffSize(memory) == 100);
    TEST_ASSERT(circularBuffFull(memory));

    for (int i = 0; i < 10; ++i) {
        TEST_ASSERT(circularBuffTail(memory));
        TEST_ASSERT(!circularBuffFull(memory));
        if ((pid = fork()) == 0){
            TEST_ASSERT(circularBuffHead(memory));
            int  err = shmdt((void*) memory);
            if (err == -1) perror("Detachment\n");
            exit(1);
        }
    }
    sleep(1);

    for (int i = 0; i < 100; ++i) {
        TEST_ASSERT(circularBuffTail(memory));
    }
    wait(NULL);
    TEST_ASSERT(circularBuffEmpty(memory));
    destroySharedMemory(memory, shmid);
    TEST_ASSERT(shmdt((void*)memory));
}

TEST_LIST = {
    {"Circular Buffer Intialiazation", test_init},
    {"Circular Buffer Insert/Remove", test_insert_remove},
    {NULL, NULL}
};

