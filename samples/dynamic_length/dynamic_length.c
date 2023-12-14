#include "circular_buffer.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static circular_buffer_t cb;
static uint8_t buffer[200];
static circular_buffer_status_t status;
static int element_len;

void print_error(circular_buffer_status_t status);

int main(void) {
    printf("CB sample - Dynamic length\n");

    status = circular_buffer_init(&cb, buffer, sizeof(buffer), true,
                                  CIRCULAR_BUFFER_DYNAMIC_LEN);
    print_error(status);
    if (status != CIRCULAR_BUFFER_SUCCESS)
        return 1;

    printf("\nstarting push...\n");

    char *str = "Ola mundo!";
    printf("push: [%d bytes] %s\n", (int)strlen(str), str);
    status = circular_buffer_push_dl(&cb, str, strlen(str));
    print_error(status);

    str = "Hello world";
    printf("push: [%d bytes] %s\n", (int)strlen(str), str);
    status = circular_buffer_push_dl(&cb, str, strlen(str));
    print_error(status);

    str = "Keyboard test qwerty 123456789";
    printf("push: [%d bytes] %s\n", (int)strlen(str), str);
    status = circular_buffer_push_dl(&cb, str, strlen(str));
    print_error(status);

    printf("\nstarting pop...\n");

    char out[100];
    while (!circular_buffer_is_empty(&cb)) {
        memset(out, 0, sizeof(out));
        status = circular_buffer_pop(&cb, (uint8_t *)out, sizeof(out),
                                     (cb_size_t *)&element_len);
        if (status == CIRCULAR_BUFFER_SUCCESS && element_len != 0)
            printf("pop: [%d bytes] %s \n", element_len, out);
    }

    return 0;
}

void print_error(circular_buffer_status_t status) {
    switch (status) {
    case CIRCULAR_BUFFER_INSUFFICIENT_SPACE:
        printf("CIRCULAR_BUFFER_INSUFFICIENT_SPACE\n");
        break;
    case CIRCULAR_BUFFER_INVALID_PARAM:
        printf("CIRCULAR_BUFFER_INVALID_PARAM\n");
        break;
    }
}
