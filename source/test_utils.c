#include "utils.h"
#include "acutest.h"
#include <stdlib.h>
#include <stdio.h>

void test_init(void) {
    SharedMem cbuffer = circularBuffInit(10000);
    TEST_ASSERT(cbuffer != NULL);
    TEST_ASSERT(circularBuffCapacity(cbuffer) == 10000);
    TEST_ASSERT(circularBuffSize(cbuffer) == 0);
    TEST_ASSERT(circularBuffEmpty(cbuffer));
    circularBuffDestroy(cbuffer);
}

void test_insert_remove(void) {
    SharedMem cbuffer = circularBuffInit(100);
    TEST_ASSERT(cbuffer != NULL);
    for(int i = 0; i < 100; ++i) {
        TEST_ASSERT(circularBuffHead(cbuffer));
        TEST_ASSERT(circularBuffSize(cbuffer) == i + 1);
    }
    TEST_ASSERT(circularBuffHead(cbuffer) == 0);
    TEST_ASSERT(circularBuffSize(cbuffer) == 100);
    for(int i = 0; i < 10; ++i) {
        TEST_ASSERT(circularBuffTail(cbuffer));
        TEST_ASSERT(circularBuffSize(cbuffer) == 99);
        TEST_ASSERT(circularBuffHead(cbuffer));
        TEST_ASSERT(circularBuffSize(cbuffer) == 100);
    }
    for (int i = 0; i < 100; ++i) {
        TEST_ASSERT(circularBuffTail(cbuffer));
        TEST_ASSERT(circularBuffSize(cbuffer) == 100 - i - 1);
    }
    TEST_ASSERT(circularBuffEmpty(cbuffer));
    circularBuffDestroy(cbuffer);
}


TEST_LIST = {
    {"Circular Buffer Intialiazation", test_init},
    {"Circular Buffer Insert/Remove", test_insert_remove},
    {NULL, NULL}
};

