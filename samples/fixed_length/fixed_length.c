#include "circular_buffer.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static circular_buffer_t cb;
static uint8_t buffer[200];
static circular_buffer_status_t status;
static int element_len;

typedef struct rgb_s {
    uint8_t r, g, b;
} rgb_t;

void print_error(circular_buffer_status_t status);

int main(void) {
    printf("CB sample - Fixed length\n");

    status =
        circular_buffer_init(&cb, buffer, sizeof(buffer), true, sizeof(rgb_t));
    print_error(status);
    if (status != CIRCULAR_BUFFER_SUCCESS)
        return 1;

    printf("\nstarting push...\n");

    rgb_t color = {.r = 0, .g = 255, .b = 135};
    status = circular_buffer_push(&cb, &color);
    print_error(status);

    printf("\nstarting pop...\n");

    rgb_t out;
    while (!circular_buffer_is_empty(&cb)) {
        status = circular_buffer_pop(&cb, (uint8_t *)&out, sizeof(out),
                                     (cb_size_t *)&element_len);
        if (status == CIRCULAR_BUFFER_SUCCESS && element_len != 0)
            printf("pop: [%d bytes] r: %d g: %d, b: %d\n", element_len, out.r,
                   out.g, out.b);
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
