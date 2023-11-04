#include "circular_buffer.h"
#include "unity.h"

void test_circular_buffer_is_empty(void) {
    circular_buffer_t cir_buffer;
    uint8_t buffer[10];

    circular_buffer_status_t ret =
        circular_buffer_init(&cir_buffer, buffer, sizeof(buffer), true, 0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "CircularBuffer creation");
    TEST_ASSERT_TRUE_MESSAGE(circular_buffer_is_empty(NULL), "Ptr  null");
    TEST_ASSERT_TRUE_MESSAGE(circular_buffer_is_empty(&cir_buffer),
                             "Buffer empty");
    cir_buffer.tail += 1;
    TEST_ASSERT_FALSE_MESSAGE(circular_buffer_is_empty(&cir_buffer),
                              "Buffer empty");
    cir_buffer.head = 5;
    cir_buffer.tail = 4;
    TEST_ASSERT_FALSE_MESSAGE(circular_buffer_is_empty(&cir_buffer),
                              "Buffer empty");
}