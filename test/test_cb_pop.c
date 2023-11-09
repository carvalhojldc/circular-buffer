#include "circular_buffer.h"
#include "unity.h"
#include <string.h>

static circular_buffer_t cb;
static uint8_t buffer[10];
static cb_size_t ret;
static cb_size_t element_len;

/**
 * @brief Test: Invalid requests
 */
void test_circular_buffer_pop_invalid_arg(void) {
    const uint8_t ELE_LEN = 1;
    ret = circular_buffer_init(&cb, buffer, sizeof(buffer), true, ELE_LEN);
    uint8_t bdata[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    memcpy(buffer, bdata, sizeof(bdata));
    cb.head = 0;
    cb.tail = sizeof(buffer) - 1;
    uint8_t o_buffer[sizeof(buffer)] = {0};

    element_len = 1;
    ret = circular_buffer_pop(NULL, o_buffer, sizeof(o_buffer), &element_len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_INVALID_PARAM, ret,
                                  "Invalid CB pointer");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, element_len, "CB element len");

    element_len = 1;
    ret = circular_buffer_pop(&cb, NULL, sizeof(o_buffer), &element_len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_INVALID_PARAM, ret,
                                  "Invalid buffer pointer");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, element_len, "CB element len");

    element_len = 1;
    ret = circular_buffer_pop(&cb, o_buffer, 0, &element_len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_INVALID_PARAM, ret,
                                  "Buffer size equal to 0");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, element_len, "CB element len");

    element_len = 1;
    cb.head = 1;
    cb.tail = 1;
    ret = circular_buffer_pop(&cb, o_buffer, sizeof(buffer), &element_len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret, "CB empty");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, element_len, "CB element len");
}

/**
 * @brief Test: 1byte-element pop
 */
void test_circular_buffer_pop_1byte(void) {
    const uint8_t ELE_LEN = 1;
    ret = circular_buffer_init(&cb, buffer, sizeof(buffer), true, ELE_LEN);
    uint8_t bdata[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    memcpy(buffer, bdata, sizeof(bdata));
    cb.head = 0;
    cb.tail = sizeof(buffer) - 1;

    uint8_t o_buffer[sizeof(buffer)] = {0};
    for (int i = 0; i < (int)sizeof(buffer) - 1; i++) {
        ret = circular_buffer_pop(&cb, &o_buffer[i], sizeof(o_buffer),
                                  &element_len);
        TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret, "CB empty");
        TEST_ASSERT_EQUAL_INT_MESSAGE(ELE_LEN, element_len, "N bytes read");
        TEST_ASSERT_EQUAL_INT_MESSAGE(bdata[i], o_buffer[i], "Pop 1 byte");
    }
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(bdata, o_buffer, sizeof(o_buffer) - 1,
                                         "Compare Pop");
    TEST_ASSERT_TRUE_MESSAGE(circular_buffer_is_empty(&cb), "Verify is empty");
    TEST_ASSERT_EQUAL_INT_MESSAGE(cb.head, sizeof(buffer) - 1, "CB head");
}

/**
 * @brief Test: 4byte-element pop with 'overwrite_oldest' tag enabled
 *          * 1st insertion : {1, 1, 1, 1, 0, 0, 0, 0, 0, 0}
 *          * 2nd insertion : {1, 1, 1, 1, 2, 2, 2, 2, 0, 0}
 *          * 3rd insertion : {3, 3, 1, 1, 2, 2, 2, 2, 3, 3}
 *                            Overwrite 1st element and move Had
 */
void test_circular_buffer_pop_4bytes(void) {
    const uint8_t ELE_LEN = 4;
    uint8_t buffer[10];
    ret = circular_buffer_init(&cb, buffer, sizeof(buffer), true, ELE_LEN);
    uint8_t bdata[] = {3, 3, 1, 1, 2, 2, 2, 2, 3, 3};
    memcpy(buffer, bdata, sizeof(buffer));
    cb.head = 4;
    cb.tail = 2;

    uint8_t o_buffer[ELE_LEN];
    for (int i = 0; i < 2; i++) {
        uint8_t expected[ELE_LEN];
        memset(expected, 2 + i, ELE_LEN);
        memset(o_buffer, 0, ELE_LEN);
        ret = circular_buffer_pop(&cb, o_buffer, ELE_LEN, &element_len);
        TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret, "CB empty");
        TEST_ASSERT_EQUAL_INT_MESSAGE(ELE_LEN, element_len, "N bytes read");
        TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(expected, o_buffer, ELE_LEN,
                                             "Pop 4 bytes");
    }
    TEST_ASSERT_TRUE_MESSAGE(circular_buffer_is_empty(&cb), "Verify is empty");
    TEST_ASSERT_EQUAL_INT_MESSAGE(cb.head, 2, "CB head");
}

/**
 * @brief Test: Push dynamic data
 *         h h 0xa
 *         h h 0xa h h 0xb 0xb
 *         0xc 0xc 0xa h h 0xb 0xb h h 0xc
 */
void test_circular_buffer_pop_dl_bytes(void) {
    uint8_t bcmp[] = {0xc, 0xc, 0xa, 0x90, 0x02, 0xb, 0xb, 0x90, 0x03, 0xc};
    uint8_t buffer[10];
    circular_buffer_init(&cb, buffer, sizeof(buffer), true, 0);
    memcpy(buffer, bcmp, sizeof(buffer));
    uint8_t data[sizeof(buffer)] = {0};
    cb.tail = 2;
    cb.head = 3;

    for (int i = 2; i <= 3; i++) {
        ret = circular_buffer_pop(&cb, data, sizeof(data), &element_len);
        TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret, "CB empty");
        TEST_ASSERT_EQUAL_INT_MESSAGE(i, element_len, "Pop Dl ret");
        uint8_t t[sizeof(buffer)];
        memset(t, 0x9 + i, sizeof(t));
        TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t, data, element_len, "t Dl data");
    }
    TEST_ASSERT_TRUE_MESSAGE(circular_buffer_is_empty(&cb), "Is empty");
}

void test_circular_buffer_pop_small_out_buffer(void) {
    uint8_t dcmp[] = {0x90, 0x09, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    uint8_t buffer[10];
    circular_buffer_init(&cb, buffer, sizeof(buffer), true, 0);
    memcpy(buffer, dcmp, sizeof(dcmp));
    uint8_t out[5] = {0};
    cb.tail = 11;
    cb.head = 0;
    ret = circular_buffer_pop(&cb, out, sizeof(out), &element_len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_INSUFFICIENT_SPACE, ret,
                                  "Pop - Small out buffer");
    TEST_ASSERT_EQUAL_INT_MESSAGE(11, cb.tail, "Pop - Small out buffer - tail");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, cb.head, "Pop - Small out buffer - head");
}