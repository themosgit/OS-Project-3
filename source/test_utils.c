#include "utils.h"
#include "acutest.h"
#include <stdlib.h>

void test_init(void) {
    CircularBuff cbuffer = circularBuffInit(10);
    TEST_ASSERT(cbuffer != NULL);
    TEST_ASSERT(circularBuffSize(cbuffer) == 10);
    TEST_ASSERT(circularBuffEmpty(cbuffer));
    circularBuffDestroy(cbuffer);
}


TEST_LIST = {
    {"Circular Buffer Intialiazation", test_init},
    {NULL, NULL}
};

