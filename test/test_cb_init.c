#include "circular_buffer.h"
#include "unity.h"
#include <string.h>

/**
 * @brief Test: circular_buffer_init with invalid args
 */
void test_circular_buffer_init_error(void) {
    circular_buffer_t cir_buffer;
    uint8_t buffer[10];
    circular_buffer_status_t ret;

    ret = circular_buffer_init(NULL, buffer, sizeof(buffer), false, 0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_INVALID_PARAM, ret,
                                  "CircularBuffer pointer");
    ret = circular_buffer_init(&cir_buffer, NULL, sizeof(buffer), false, 0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_INVALID_PARAM, ret,
                                  "CircularBuffer buffer");
    ret = circular_buffer_init(&cir_buffer, buffer, 0, false, 0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_INVALID_PARAM, ret,
                                  "CircularBuffer buffer size");
}

/**
 * @brief Test: circular_buffer_init creating a valid object
 */
void test_circular_buffer_init_success(void) {
    for (int i = 0; i < 2; i++) {
        circular_buffer_t cir_buffer;
        uint8_t buffer[10 + i * 10];
        uint8_t clean_buffer[sizeof(buffer)];
        memset(clean_buffer, 0, sizeof(clean_buffer));

        circular_buffer_status_t ret =
            circular_buffer_init(&cir_buffer, buffer, sizeof(buffer), i, i);
        TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                      "CircularBuffer creation");
        TEST_ASSERT_EQUAL_PTR_MESSAGE(buffer, cir_buffer.buffer.ptr,
                                      "Buffer pointer");
        TEST_ASSERT_EQUAL_INT_MESSAGE(sizeof(buffer), cir_buffer.buffer.size,
                                      "Buffer size");
        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
            clean_buffer, buffer, sizeof(buffer), "Buffer initialization");
        TEST_ASSERT_EQUAL_INT_MESSAGE(i, cir_buffer.buffer.overwrite_oldest,
                                      "Overwrite oldest");
        TEST_ASSERT_EQUAL_INT_MESSAGE(i, cir_buffer.buffer.element_len,
                                      "Element lengh");
        TEST_ASSERT_TRUE_MESSAGE(cir_buffer.tail == 0, "Tail initialization");
        TEST_ASSERT_TRUE_MESSAGE(cir_buffer.head == 0, "Head initialization");
    }
}
