#include "circular_buffer.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static circular_buffer_t cb;
static uint8_t buffer[200];
static circular_buffer_status_t status;

void print_error(circular_buffer_status_t status);

int main(void) {
    status = circular_buffer_init(&cb, buffer, sizeof(buffer), true, 0);
    print_error(status);
    if (status != CIRCULAR_BUFFER_SUCCESS)
        return 1;

    char *str = "Ola mundo!";
    printf("push: %s\n", str);
    status = circular_buffer_push_dl(&cb, str, strlen(str));
    print_error(status);

    str = "Hello world";
    printf("push: %s\n", str);
    status = circular_buffer_push_dl(&cb, str, strlen(str));
    print_error(status);

    str = "Keyboard test qwerty 123456789";
    printf("push: %s\n", str);
    status = circular_buffer_push_dl(&cb, str, strlen(str));
    print_error(status);

    printf("\nstarting pop...\n");

    char out[100];
    while (!circular_buffer_is_empty(&cb)) {
        memset(out, 0, sizeof(out));
        cb_size_t ret = circular_buffer_pop(&cb, (uint8_t *)out, sizeof(out));
        if (ret != 0)
            printf("pop: %s\n", out);
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
