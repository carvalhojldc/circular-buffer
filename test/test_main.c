#include "circular_buffer.h"
#include "unity.h"
#include <stdint.h>
#include <string.h>

void test_circular_buffer_init_error(void);
void test_circular_buffer_init_success(void);

void test_circular_buffer_is_empty(void);

void test_circular_buffer_is_full(void);

void test_circular_buffer_has_enough_space_NOT_overwrite_oldest(void);
void test_circular_buffer_has_enough_space_overwrite_oldest(void);

void test_circular_buffer_push_error(void);
void test_circular_buffer_push_1byte_overwrite_oldest(void);
void test_circular_buffer_push_1byte_NOT_overwrite_oldest(void);
void test_circular_buffer_push_4byte_overwrite_oldest(void);
void test_circular_buffer_push_4byte_NOT_overwrite_oldest(void);

void test_circular_buffer_push_dl_header(void);
void test_circular_buffer_push_dl(void);
void test_circular_buffer_push_dl_NOT_overwrite(void);
void test_circular_buffer_push_dl_overwrite(void);

void test_circular_buffer_pop_invalid_arg(void);
void test_circular_buffer_pop_1byte(void);
void test_circular_buffer_pop_4bytes(void);
void test_circular_buffer_pop_dl_bytes(void);

void setUp(void) {}

void tearDown(void) {}

int main(void) {
    printf("\nCircular Buffer - Unity tests\r\n\n");

    UNITY_BEGIN();

    RUN_TEST(test_circular_buffer_init_error);
    RUN_TEST(test_circular_buffer_init_success);

    RUN_TEST(test_circular_buffer_is_empty);
    RUN_TEST(test_circular_buffer_is_full);

    RUN_TEST(test_circular_buffer_has_enough_space_overwrite_oldest);
    RUN_TEST(test_circular_buffer_has_enough_space_NOT_overwrite_oldest);

    RUN_TEST(test_circular_buffer_push_error);
    RUN_TEST(test_circular_buffer_push_1byte_overwrite_oldest);
    RUN_TEST(test_circular_buffer_push_1byte_NOT_overwrite_oldest);
    RUN_TEST(test_circular_buffer_push_4byte_overwrite_oldest);
    RUN_TEST(test_circular_buffer_push_4byte_NOT_overwrite_oldest);

    RUN_TEST(test_circular_buffer_push_dl_header);
    RUN_TEST(test_circular_buffer_push_dl);
    RUN_TEST(test_circular_buffer_push_dl_NOT_overwrite);
    RUN_TEST(test_circular_buffer_push_dl_overwrite);

    RUN_TEST(test_circular_buffer_pop_invalid_arg);
    RUN_TEST(test_circular_buffer_pop_1byte);
    RUN_TEST(test_circular_buffer_pop_4bytes);
    RUN_TEST(test_circular_buffer_pop_dl_bytes);

    return UNITY_END();
}