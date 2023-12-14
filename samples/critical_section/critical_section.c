#include "circular_buffer.h"
#include "circular_buffer_porting.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <pthread.h>
#include <unistd.h>

static unsigned data_to_insert = 0;
static circular_buffer_t cb;
static circular_buffer_status_t status;
static cb_size_t element_len;
static uint8_t buffer[1500];
static uint8_t b_backup[sizeof(buffer)];
static char out[1000];
static char data[1000];

volatile bool push_working = true;
pthread_t tpush, tpop;
pthread_mutex_t mutex;

#define BUFFER_DUMP_ON_ERROR (1)

/* Porting START */
void circular_buffer_porting_CRITICAL_ENTER(void) {
    pthread_mutex_lock(&mutex);
}

void circular_buffer_porting_CRITICAL_EXIT(void) {
    pthread_mutex_unlock(&mutex);
}
/* Porting END */

void dump_buffer(uint8_t buffer[], const int size);
void dump(void);
int push_data(const int len);
int pop_data(void);

void *thread_pop(void *vargp) {
    while (push_working || !circular_buffer_is_empty(&cb)) {
        usleep(100);
        if (pop_data()) { /* pop */
            return (NULL);
        }
    }
}

void *thread_push(void *vargp) {
    int i, j, nt;

    int n_times = 10000;
    int rotations = 1000;

    for (nt = 1; nt <= n_times; nt++) {
        data_to_insert = 0;
        status = circular_buffer_init(&cb, buffer, sizeof(buffer), true,
                                      CIRCULAR_BUFFER_DYNAMIC_LEN);
        if (status != CIRCULAR_BUFFER_SUCCESS) {
            printf("ERROR circular_buffer_init %d\n", status);
            return (NULL);
        }

        for (i = 0; i < rotations; i++) {
            for (j = 0; j < (rand() % 4) + 2; j++)
                if (push_data((rand() % 265) + 2)) { /* push */
                    return (NULL);
                }
        }
        printf("End %d\n\n", nt);
    }

    printf(">\n");

    push_working = false;
}

int main(void) {
    srand(time(NULL));

    pthread_mutex_init(&mutex, NULL);
    pthread_create(&tpop, NULL, thread_pop, (void *)&tpop);
    pthread_create(&tpush, NULL, thread_push, (void *)&tpush);

    pthread_exit(NULL);

    return 0;
}

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
        pthread_cancel(tpush);
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
