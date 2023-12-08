#include "circular_buffer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static unsigned data_to_insert = 0;
static circular_buffer_t cb;
static circular_buffer_status_t status;
static cb_size_t element_len;
static uint8_t buffer[1500];
static uint8_t b_backup[sizeof(buffer)];
static char out[1000];
static char data[1000];

#define BUFFER_DUMP_ON_ERROR (1)

void dump_buffer(uint8_t buffer[], const int size) {
    int i;
    printf("\n");
    for (i = 0; i < size; i++) {
        if (i % 20 == 0)
            printf("\n");
        printf("(%d)%X ", i, buffer[i]);
    }
    printf("\n");
}

void dump(void) {
    printf("\n\tBefore insertion:\n");
    dump_buffer(buffer, sizeof(buffer));
    printf("\n\tAfter insertion:\n");
    dump_buffer(b_backup, sizeof(b_backup));
}

int push_data(const int len) {
    memset(data, ++data_to_insert, sizeof(data));
    printf("+ push (%X) len %d\n", data_to_insert, len);
#if BUFFER_DUMP_ON_ERROR
    memcpy(b_backup, buffer, sizeof(b_backup));
#endif
    status = circular_buffer_push_dl(&cb, data, len);
    if (status != CIRCULAR_BUFFER_SUCCESS) {
        printf("ERROR circular_buffer_push_dl %d\n", status);
#if BUFFER_DUMP_ON_ERROR
        dump();
#endif
        return 1;
    }
    printf("head %d tail %d\n", CB_HEAD((&cb)), CB_TAIL((&cb)));

    return 0;
}

int pop_data(void) {
    printf("\n- pop");
    status = circular_buffer_pop(&cb, out, sizeof(out), &element_len);
    if (status != CIRCULAR_BUFFER_SUCCESS) {
        printf("ERROR circular_buffer_pop %d\n", status);
#if BUFFER_DUMP_ON_ERROR
        dump();
#endif
        return 1;
    }
    printf(" (%X) len %d\n\thead %d tail %d\n\n", out[0], element_len,
           CB_HEAD((&cb)), CB_TAIL((&cb)));

    return 0;
}

int main(void) {
    int i, j, nt;
    srand(time(NULL));

    int n_times = 100;
    int rotations = 500;
    int element_len = 125;

    for (nt = 1; nt <= n_times; nt++) {
        data_to_insert = 0;
        status = circular_buffer_init(&cb, buffer, sizeof(buffer), true,
                                      element_len);
        if (status != CIRCULAR_BUFFER_SUCCESS) {
            printf("ERROR circular_buffer_init %d\n", status);
            return 1;
        }

        for (i = 0; i < rotations; i++) {
            for (j = 0; j < (rand() % 4) + 2; j++)
                if (push_data(element_len)) { /* push */
                    return 1;
                }

            if (pop_data()) { /* pop */
                return nt;
            }
        }
        printf("End %d\n\n", nt);
    }

    printf(">\n");

    return 0;
}