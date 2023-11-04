#include "circular_buffer.h"
#include "unity.h"

void test_circular_buffer_is_full(void) {
    circular_buffer_t cb;
    uint8_t buffer[9];
    uint8_t binsert[sizeof(buffer)];

    for (int i = 0; i < (int)sizeof(buffer); i++)
        binsert[i] = i + 1;

    TEST_ASSERT_TRUE_MESSAGE(circular_buffer_is_full(NULL), "Ptr null");
    circular_buffer_status_t ret = circular_buffer_init(
        &cb, buffer, sizeof(buffer), true, sizeof(buffer) - 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "CircularBuffer creation");
    TEST_ASSERT_FALSE_MESSAGE(circular_buffer_is_full(&cb), "Buffer empty");
    ret = circular_buffer_push(&cb, binsert);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "CircularBuffer creation");
    TEST_ASSERT_TRUE_MESSAGE(circular_buffer_is_full(&cb), "Buffer full");
}