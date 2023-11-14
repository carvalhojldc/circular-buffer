#include "circular_buffer.h"
#include "unity.h"
#include <string.h>

/**
 * @brief Test invalid push
 */
void test_circular_buffer_push_error(void) {
    circular_buffer_t cb;
    uint8_t buffer[10];

    circular_buffer_status_t ret =
        circular_buffer_init(&cb, buffer, sizeof(buffer), true, 0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "CircularBuffer creation");

    const char *test = "Hello World";
    ret = circular_buffer_push(NULL, (uint8_t *)test);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_INVALID_PARAM, ret,
                                  "Push CircularBuffer pointer");
    ret = circular_buffer_push(&cb, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_INVALID_PARAM, ret,
                                  "Push CircularBuffer data");
}

/**
 * @brief Test: 1byte-element push with 'overwrite_oldest' tag enabled
 */
void test_circular_buffer_push_1byte_overwrite_oldest(void) {
    circular_buffer_t cb;
    uint8_t buffer[10];
    uint8_t bcmp[sizeof(buffer)] = {
        0xB, 0xC, 0x3, 0x4, 0x5,
        0x6, 0x7, 0x8, 0x9, 0xA}; /* Rotate and isert two more elements */

    circular_buffer_status_t ret =
        circular_buffer_init(&cb, buffer, sizeof(buffer), true, 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "CircularBuffer creation");
    for (int i = 0; i < (int)sizeof(buffer) + 2; i++) {
        uint8_t data = 0x1 + i;
        ret = circular_buffer_push(&cb, &data);
        TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                      "Push CircularBuffer");
        TEST_ASSERT_EQUAL_HEX8_MESSAGE(data, buffer[i % sizeof(buffer)],
                                       "Push Error");
        TEST_ASSERT_FALSE_MESSAGE(circular_buffer_is_empty(&cb),
                                  "Buffer empty");
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, CB_HEAD((&cb)), "Head validation");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, CB_TAIL((&cb)), "Tail validation");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(bcmp, buffer, sizeof(buffer),
                                         "Buffer validation");
}

/**
 * @brief Test: 1byte-element push with 'overwrite_oldest' tag NOT enabled
 */
void test_circular_buffer_push_1byte_NOT_overwrite_oldest(void) {
    circular_buffer_t cb;
    uint8_t buffer[10];
    uint8_t bcmp[sizeof(buffer)] = {0x1, 0x2, 0x3, 0x4, 0x5,
                                    0x6, 0x7, 0x8, 0x9, 0x0};

    circular_buffer_status_t ret =
        circular_buffer_init(&cb, buffer, sizeof(buffer), false, 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "CircularBuffer creation");
    for (int i = 0; i <= (int)sizeof(buffer); i++) {
        uint8_t data = 0x1 + i;

        if (i < ((int)sizeof(buffer) - 1)) {
            TEST_ASSERT_FALSE_MESSAGE(circular_buffer_is_full(&cb),
                                      "Buffer full");
            ret = circular_buffer_push(&cb, &data);
            TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                          "Push CircularBuffer");
            TEST_ASSERT_FALSE_MESSAGE(circular_buffer_is_empty(&cb),
                                      "Buffer empty");
        } else {
            /* overwrite_oldest NOT enabled */
            TEST_ASSERT_TRUE_MESSAGE(circular_buffer_is_full(&cb),
                                     "Buffer full");
            ret = circular_buffer_push(&cb, &data);
            TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_INSUFFICIENT_SPACE,
                                          ret, "Push CircularBuffer");
        }
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, CB_HEAD((&cb)), "Head validation");
    TEST_ASSERT_EQUAL_INT_MESSAGE(9, CB_TAIL((&cb)), "Tail validation");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(bcmp, buffer, sizeof(buffer),
                                         "Buffer validation");
}

/**
 * @brief Test: 4byte-element push with 'overwrite_oldest' tag NOT enabled
 *        A 10 bytes buffer cannot accept more the 2 of these elements,
 *        and the last 2 bytes of the buffer must be empty
 */
void test_circular_buffer_push_4byte_NOT_overwrite_oldest(void) {
    circular_buffer_t cb;
    uint8_t buffer[10];
    const uint8_t SIZE = 4; /* 4byte-element */
    uint8_t bcmp[sizeof(buffer)] = {1, 1, 1, 1, 2, 2, 2, 2, 0, 0};

    circular_buffer_status_t ret =
        circular_buffer_init(&cb, buffer, sizeof(buffer), false, SIZE);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "CircularBuffer creation");
    /* 10 bytes buffer - 3 insetions to test rotation with 4byte-element */
    for (int i = 0; i < 3; i++) {
        uint8_t data[SIZE];
        memset(data, i + 1, SIZE);
        ret = circular_buffer_push(&cb, &data);
        if (i < 2) {
            TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                          "Push CircularBuffer");
            TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&data, buffer + (SIZE * i), SIZE,
                                             "Push Error");
            TEST_ASSERT_FALSE_MESSAGE(circular_buffer_is_empty(&cb),
                                      "Buffer empty");
        } else {
            /* overwrite_oldest NOT enabled */
            TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_INSUFFICIENT_SPACE,
                                          ret, "Push CircularBuffer");
        }
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, CB_HEAD((&cb)), "Head validation");
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, CB_TAIL((&cb)), "Tail validation");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(bcmp, buffer, sizeof(buffer),
                                         "Buffer validation");
}

/**
 * @brief Test: 4byte-element push with 'overwrite_oldest' tag enabled
 *        The buffer can accept all insertions and the Tail must rotate to
 *        insert the 3rd element and overwrite the first
 *          * 1st insertion : {10, 11, 12, 13, 00, 00, 00, 00, 00, 00}
 *          * 2nd insertion : {10, 11, 12, 13, 20, 21, 22, 23, 00, 00}
 *          * 3rd insertion : {32, 33, 12, 13, 20, 21, 22, 23, 30, 31}
 *                            Overwrite 1st element and move Had
 */
void test_circular_buffer_push_4byte_overwrite_oldest(void) {
    circular_buffer_t cb;
    uint8_t buffer[10];
    const uint8_t SIZE = 4;
    uint8_t bcmp[sizeof(buffer)] = {32, 33, 12, 13, 20, 21, 22, 23, 30, 31};

    circular_buffer_status_t ret =
        circular_buffer_init(&cb, buffer, sizeof(buffer), true, SIZE);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "CircularBuffer creation");
    /* 10 bytes buffer - 3 insetions to test rotation with 4byte-element */
    uint8_t byte = 0;
    for (int i = 0; i < 3; i++) {
        uint8_t data[SIZE];
        byte += 10;
        for (int j = 0; j < SIZE; j++)
            data[j] = byte + j;
        ret = circular_buffer_push(&cb, &data);
        TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                      "Push CircularBuffer");
        if (i < 2) {
            TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(data, buffer + (SIZE * i),
                                                 SIZE, "Push Error");
            TEST_ASSERT_FALSE_MESSAGE(circular_buffer_is_empty(&cb),
                                      "Buffer empty");
        } else {
            /* Validate rotation from 3rd element*/
            TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(data + SIZE / 2, buffer,
                                                 SIZE / 2, "Push Error start");
            TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(data, buffer + (SIZE * i),
                                                 SIZE / 2, "Push Error end");
        }
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, CB_HEAD((&cb)), "Head validation");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, CB_TAIL((&cb)), "Tail validation");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(bcmp, buffer, sizeof(buffer),
                                         "Buffer validation");
}
