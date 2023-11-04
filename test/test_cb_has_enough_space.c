#include "circular_buffer.h"
#include "unity.h"

void test_circular_buffer_has_enough_space_NOT_overwrite_oldest(void) {
    circular_buffer_t cb;
    circular_buffer_status_t ret;
    uint8_t buffer[10];

    ret = circular_buffer_init(&cb, buffer, sizeof(buffer), false, 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "CircularBuffer creation");
    TEST_ASSERT_FALSE_MESSAGE(
        circular_buffer_has_enough_space(NULL, sizeof(buffer)),
        "CB pinter == NULL");
    TEST_ASSERT_TRUE_MESSAGE(circular_buffer_has_enough_space(&cb, 0),
                             "Elemment len == 0");
    TEST_ASSERT_FALSE_MESSAGE(
        circular_buffer_has_enough_space(&cb, sizeof(buffer)),
        "Element == buffer size");
    TEST_ASSERT_TRUE_MESSAGE(
        circular_buffer_has_enough_space(&cb, sizeof(buffer) - 1),
        "Elemment len = buffer-1");
    TEST_ASSERT_TRUE_MESSAGE(
        circular_buffer_has_enough_space(&cb, sizeof(buffer) / 2),
        "Elemment len = half of buffer");
    TEST_ASSERT_TRUE_MESSAGE(circular_buffer_has_enough_space(&cb, 1),
                             "Elemment len = 1");

    ret = circular_buffer_init(&cb, buffer, sizeof(buffer), false, 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "CircularBuffer creation");
    cb.tail = sizeof(buffer) - 1;
    cb.head = 0;
    TEST_ASSERT_FALSE(circular_buffer_has_enough_space(&cb, 1));
    cb.tail = sizeof(buffer) - 1;
    cb.head = 1;
    TEST_ASSERT_TRUE(circular_buffer_has_enough_space(&cb, 1));

    ret = circular_buffer_init(&cb, buffer, sizeof(buffer), false, 4);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "CircularBuffer creation");
    cb.tail = sizeof(buffer) - 3;
    cb.head = 0;
    TEST_ASSERT_FALSE(circular_buffer_has_enough_space(&cb, 4));
    cb.tail = sizeof(buffer) - 3;
    cb.head = 2;
    TEST_ASSERT_TRUE(circular_buffer_has_enough_space(&cb, 4));
}

void test_circular_buffer_has_enough_space_overwrite_oldest(void) {
    circular_buffer_t cb;
    circular_buffer_status_t ret;
    uint8_t buffer[10];

    for (int i = 0; i < 2; i++)
        ret = circular_buffer_init(&cb, buffer, sizeof(buffer), true, 2 * i);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "CircularBuffer creation");
    TEST_ASSERT_TRUE_MESSAGE(circular_buffer_has_enough_space(&cb, 0),
                             "Elemment len == 0");
    TEST_ASSERT_TRUE_MESSAGE(circular_buffer_has_enough_space(&cb, 5),
                             "Elemment len == 0");
    TEST_ASSERT_TRUE_MESSAGE(
        circular_buffer_has_enough_space(&cb, sizeof(buffer) - 1),
        "Elemment len == buffer - 1");
    TEST_ASSERT_FALSE_MESSAGE(
        circular_buffer_has_enough_space(&cb, sizeof(buffer)),
        "Elemment len == buffer");
    TEST_ASSERT_FALSE_MESSAGE(
        circular_buffer_has_enough_space(&cb, sizeof(buffer) + 100),
        "Elemment len == bigger");
}
