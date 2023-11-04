#include "circular_buffer.h"
#include "unity.h"
#include <string.h>

static circular_buffer_t cb;
static cb_size_t ret;

/**
 * @brief Test: Validate header
 */
void test_circular_buffer_push_dl_header(void) {
    uint8_t buffer[2000];
    /* Data len uses 12 bits: */
    uint8_t data[1000] = {0};
    circular_buffer_init(&cb, buffer, sizeof(buffer), false, 0);

    circular_buffer_push_dl(&cb, data, sizeof(data));
    uint8_t header[] = {0x93, 0xE8};
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(header, buffer, sizeof(header),
                                         "Header validation");
}

/**
 * @brief Test: Push dynamic data
 *         h h 0xa
 *         h h 0xa h h 0xb 0xb
 *         h h 0xa h h 0xb 0xb h h 0xc 0xc 0xc
 */
void test_circular_buffer_push_dl(void) {
    uint8_t bcmp[] = {0x90, 0x01, 0xa, 0x90, 0x02, 0xb,
                      0xb,  0x90, 0x3, 0xc,  0xc,  0xc};
    uint8_t buffer[15];
    circular_buffer_init(&cb, buffer, sizeof(buffer), false, 0);
    uint8_t data[sizeof(buffer)] = {0};

    for (int i = 1; i <= 3; i++) {
        memset(data, 0x9 + i, i);
        ret = circular_buffer_push_dl(&cb, data, i);
        TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                      "Push CircularBuffer");
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(12, CB_TAIL((&cb)), "Tail position");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(bcmp, buffer, sizeof(bcmp),
                                         "Buffer insertion");
}

/**
 * @brief Test: Push dynamic data
 *         h h 0xa
 *         h h 0xa h h 0xb 0xb
 */
void test_circular_buffer_push_dl_NOT_overwrite(void) {
    uint8_t bcmp[] = {0x90, 0x01, 0xa, 0x90, 0x02, 0xb, 0xb, 0x0, 0x0, 0x0};
    uint8_t buffer[10];
    circular_buffer_init(&cb, buffer, sizeof(buffer), false, 0);
    uint8_t data[sizeof(buffer)] = {0};

    for (int i = 1; i <= 3; i++) {
        memset(data, 0x9 + i, i);
        ret = circular_buffer_push_dl(&cb, data, i);
        if (i <= 2)
            TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                          "Push CircularBuffer");
        else
            TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_INSUFFICIENT_SPACE,
                                          ret, "Push CircularBuffer Full");
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(7, CB_TAIL((&cb)), "Tail position");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(bcmp, buffer, sizeof(bcmp),
                                         "Buffer insertion");
}

/**
 * @brief Test: Push dynamic data
 *         h h 0xa
 *         h h 0xa h h 0xb 0xb
 *         0xc 0xc 0xa h h 0xb 0xb h h 0xc
 */
void test_circular_buffer_push_dl_overwrite(void) {
    uint8_t bcmp[] = {0xc, 0xc, 0xa, 0x90, 0x02, 0xb, 0xb, 0x90, 0x03, 0xc};
    uint8_t buffer[10];
    circular_buffer_init(&cb, buffer, sizeof(buffer), true, 0);
    uint8_t data[sizeof(buffer)] = {0};

    for (int i = 1; i <= 3; i++) {
        memset(data, 0x9 + i, i);
        ret = circular_buffer_push_dl(&cb, data, i);
        TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                      "Push CircularBuffer");
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, CB_TAIL((&cb)), "Tail position");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(bcmp, buffer, sizeof(bcmp),
                                         "Buffer insertion");
}