#include "circular_buffer.h"
#include "unity.h"
#include <string.h>

void test_cb_dynamic_head_rotation(void) {
    circular_buffer_t cb;
    circular_buffer_status_t ret;
    uint8_t buffer[10];
    uint8_t data[10];
    int len;

    for (int i = 0; i < 4; i++) {
        ret = circular_buffer_init(&cb, buffer, sizeof(buffer), true,
                                   CIRCULAR_BUFFER_DYNAMIC_LEN);
        TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                      "test_cb_dynamic_head_rotation");

        /* h h 1 1 0 0 0 0 0 0 */
        len = 2;
        memset(data, 1, len);
        ret = circular_buffer_push_dl(&cb, data, len);
        TEST_ASSERT_EQUAL_INT_MESSAGE(
            CIRCULAR_BUFFER_SUCCESS, ret,
            "test_cb_dynamic_head_rotation push: len 2");

        /* i = 0 > h h 1 1 h h 2 2 0 0
         * i = 1 > h h 1 1 h h 2 2 2 0
         * i = 2 > h h 1 1 h h 2 2 2 2
         * i = 3 > 2 h 1 1 h h 2 2 2 2
         */
        len = 2 + i;
        memset(data, 2, len);
        ret = circular_buffer_push_dl(&cb, data, len);
        TEST_ASSERT_EQUAL_INT_MESSAGE(
            CIRCULAR_BUFFER_SUCCESS, ret,
            "test_cb_dynamic_head_rotation push: len 3");

        /* i = 0 > 3 3 3 1 h h 2 2 h h
         * i = 1 > h 3 3 3 h h 2 2 2 h
         * i = 2 > h h 3 3 3 h 2 2 2 2
         * i = 3 > 2 h h 3 3 3 2 2 2 2
         */
        len = 3;
        memset(data, 3, len);
        ret = circular_buffer_push_dl(&cb, data, len);
        TEST_ASSERT_EQUAL_INT_MESSAGE(
            CIRCULAR_BUFFER_SUCCESS, ret,
            "test_cb_dynamic_head_rotation push: len 1");

        if (i == 3) {
            /* i = 3 > 2 h h 3 3 3 h h 4 2 */
            len = 1;
            memset(data, 4, len);
            ret = circular_buffer_push_dl(&cb, data, len);

            TEST_ASSERT_EQUAL_INT_MESSAGE(
                CIRCULAR_BUFFER_SUCCESS, ret,
                "test_cb_dynamic_head_rotation push: len 2");
        }

        int head = 0, tail = 0;
        switch (i) {
        case 0:
            head = 4;
            tail = 3;
            break;
        case 1:
            head = 9;
            tail = 4;
            break;
        case 2:
            head = 0;
            tail = 5;
            break;
        case 3:
            head = 1;
            tail = 9;
            break;
        default:
            break;
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(tail, CB_TAIL((&cb)),
                                      "test_cb_dynamic_head_rotation tail");
        TEST_ASSERT_EQUAL_INT_MESSAGE(head, CB_HEAD((&cb)),
                                      "test_cb_dynamic_head_rotation head");
    }
}

void test_cb_dynamic_data_rotation(void) {
    circular_buffer_t cb;
    cb_size_t out_len;
    circular_buffer_status_t ret;
    uint8_t buffer[15];
    uint8_t data[sizeof(buffer)];
    uint8_t out[sizeof(buffer)];
    int len;

    ret = circular_buffer_init(&cb, buffer, sizeof(buffer), true, 0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "test_cb_dynamic_data_rotation");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, CB_TAIL((&cb)),
                                  "test_cb_dynamic_data_rotation tail");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, CB_HEAD((&cb)),
                                  "test_cb_dynamic_data_rotation head");

    /* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14
     * hh hh  1  1  0  0  0  0  0  0  0  0  0  0  0
     */
    uint8_t cmp1[] = {0x90, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    len = 2;
    memset(data, 1, len);
    ret = circular_buffer_push_dl(&cb, data, len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "test_cb_dynamic_data_rotation push: 1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, CB_TAIL((&cb)),
                                  "test_cb_dynamic_data_rotation push: 1 tail");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, CB_HEAD((&cb)),
                                  "test_cb_dynamic_data_rotation push: 1 head");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(
        cmp1, buffer, sizeof(buffer),
        "test_cb_dynamic_data_rotation push: 1 buffer");

    /* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14
     * hh hh  1  1 hh hh  2  0  0  0  0  0  0  0  0
     */
    uint8_t cmp2[] = {0x90, 2, 1, 1, 0x90, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0};
    len = 1;
    memset(data, 2, len);
    ret = circular_buffer_push_dl(&cb, data, len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "test_cb_dynamic_data_rotation push: 2");
    TEST_ASSERT_EQUAL_INT_MESSAGE(7, CB_TAIL((&cb)),
                                  "test_cb_dynamic_data_rotation push: 2 tail");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, CB_HEAD((&cb)),
                                  "test_cb_dynamic_data_rotation push: 2 head");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(
        cmp2, buffer, sizeof(buffer),
        "test_cb_dynamic_data_rotation push: 2 buffer");

    /* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14
     *  x  x  x  x hh hh  2  0  0  0  0  0  0  0  0
     */
    out_len = 0;
    ret = circular_buffer_pop(&cb, out, sizeof(out), &out_len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, out_len,
                                  "test_cb_dynamic_data_rotation pop: 1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, out[0],
                                  "test_cb_dynamic_data_rotation pop: 1 out");
    TEST_ASSERT_EQUAL_INT_MESSAGE(7, CB_TAIL((&cb)),
                                  "test_cb_dynamic_data_rotation pop: 1 tail");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, CB_HEAD((&cb)),
                                  "test_cb_dynamic_data_rotation pop: 1 head");

    /* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14
     *  x  x  x  x hh hh  2 hh hh  3  3  3  0  0  0
     */
    uint8_t cmp3[] = {0x90, 2, 1, 1, 0x90, 1, 2, 0x90, 3, 3, 3, 3, 0, 0, 0};
    len = 3;
    memset(data, 3, len);
    ret = circular_buffer_push_dl(&cb, data, len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "test_cb_dynamic_data_rotation push: 3");
    TEST_ASSERT_EQUAL_INT_MESSAGE(12, CB_TAIL((&cb)),
                                  "test_cb_dynamic_data_rotation push: 3 tail");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, CB_HEAD((&cb)),
                                  "test_cb_dynamic_data_rotation push: 3 head");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(
        cmp3, buffer, sizeof(buffer),
        "test_cb_dynamic_data_rotation push: 3 buffer");

    /* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14
     *  4  4  x  x hh hh  2 hh hh  3  3  3 hh hh  4
     */
    uint8_t cmp4[] = {4, 4, 1, 1, 0x90, 1, 2, 0x90, 3, 3, 3, 3, 0x90, 3, 4};
    len = 3;
    memset(data, 4, len);
    ret = circular_buffer_push_dl(&cb, data, len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "test_cb_dynamic_data_rotation push: 4");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, CB_TAIL((&cb)),
                                  "test_cb_dynamic_data_rotation push: 4 tail");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, CB_HEAD((&cb)),
                                  "test_cb_dynamic_data_rotation push: 4 head");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(
        cmp4, buffer, sizeof(buffer),
        "test_cb_dynamic_data_rotation push: 4 buffer");

    /* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14
     *  4  4 hh hh  5  5  5  5  5  5  3  3 hh hh  4
     */
    uint8_t cmp5[] = {4, 4, 0x90, 6, 5, 5, 5, 5, 5, 5, 3, 3, 0x90, 3, 4};
    len = 6;
    memset(data, 5, len);
    ret = circular_buffer_push_dl(&cb, data, len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "test_cb_dynamic_data_rotation push: 5");
    TEST_ASSERT_EQUAL_INT_MESSAGE(10, CB_TAIL((&cb)),
                                  "test_cb_dynamic_data_rotation push: 5 tail");
    TEST_ASSERT_EQUAL_INT_MESSAGE(12, CB_HEAD((&cb)),
                                  "test_cb_dynamic_data_rotation push: 5 head");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(
        cmp5, buffer, sizeof(buffer),
        "test_cb_dynamic_data_rotation push: 5 buffer");

    /* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14
     *  x  x hh hh  5  5  5  5  5  5  3  3  x  x  x
     */
    out_len = 0;
    ret = circular_buffer_pop(&cb, out, sizeof(out), &out_len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "test_cb_dynamic_data_rotation pop 4 ret");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, out[0],
                                  "test_cb_dynamic_data_rotation pop: 4 out");
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, out_len,
                                  "test_cb_dynamic_data_rotation pop: 4");
    TEST_ASSERT_EQUAL_INT_MESSAGE(10, CB_TAIL((&cb)),
                                  "test_cb_dynamic_data_rotation pop: 4 tail");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, CB_HEAD((&cb)),
                                  "test_cb_dynamic_data_rotation pop: 4 head");

    /* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14
     *  6  6 hh hh  5  5  5  5  5  5 hh hh  6  6  6
     */
    uint8_t cmp6[] = {6, 6, 0x90, 6, 5, 5, 5, 5, 5, 5, 0x90, 5, 6, 6, 6};
    len = 5;
    memset(data, 6, len);
    ret = circular_buffer_push_dl(&cb, data, len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "test_cb_dynamic_data_rotation push: 6");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, CB_TAIL((&cb)),
                                  "test_cb_dynamic_data_rotation push: 6 tail");
    TEST_ASSERT_EQUAL_INT_MESSAGE(10, CB_HEAD((&cb)),
                                  "test_cb_dynamic_data_rotation push: 6 head");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(
        cmp6, buffer, sizeof(buffer),
        "test_cb_dynamic_data_rotation push: 6 buffer");

    /* 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14
     *  6  6 hh hh  7  7  7  7  7  7 hh hh  6  6  6
     */
    uint8_t cmp7[] = {6, 6, 0x90, 7, 7, 7, 7, 7, 7, 7, 7, 5, 6, 6, 6};
    len = 7;
    memset(data, 7, len);
    ret = circular_buffer_push_dl(&cb, data, len);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CIRCULAR_BUFFER_SUCCESS, ret,
                                  "test_cb_dynamic_data_rotation push: 7");
    TEST_ASSERT_EQUAL_INT_MESSAGE(11, CB_TAIL((&cb)),
                                  "test_cb_dynamic_data_rotation push: 7 tail");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, CB_HEAD((&cb)),
                                  "test_cb_dynamic_data_rotation push: 7 head");
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(
        cmp7, buffer, sizeof(buffer),
        "test_cb_dynamic_data_rotation push: 7 buffer");
}