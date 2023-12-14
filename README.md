# Circular Buffer

A simple circular buffer to be used with fixed and dynamic data length.

## Usage

### Initialization

A storage buffer must be allocated and passed at Circular Buffer initialization.

The Circular Buffer will be with:
* dynamic data length: `element_len` must be 0
* fixed data length: `element_len` must be the size of the element

```c
#include "circular_buffer.h"

circular_buffer_t cb;
uint8_t storage_buffer[200];
circular_buffer_status_t status;
cb_size_t element_len;
char out[50] = {0};

status = circular_buffer_init(&cb,
                              storage_buffer,
                              sizeof(storage_buffer),
                              true, /* true -> overwrite_oldest */
                              0); /* 0 or CIRCULAR_BUFFER_DYNAMIC_LEN -> element_len (dynamic length) */

char *str = "Hello";
status = circular_buffer_push_dl(&cb, str, strlen(str));

str = " World!!!!";
status = circular_buffer_push_dl(&cb, str, strlen(str));
...
status = circular_buffer_pop(&cb, (uint8_t*)out, sizeof(out), &element_len);
printf("len: %d\ndata: %s\n", element_len, out);

```

### Examples
Available in the `samples` folder.

## Porting

### Critical section

When enabling the `overwrite_oldest` option in environments with parallelism, it is recommended to enable the critical section protection to avoid data races when updating the `head`.

To do this, the variable `CIRCULAR_BUFFER_USE_CRITICAL` must be set to `1` during the build process.

Activate the option on your makefile:
```bash
CFLAGS=-DCIRCULAR_BUFFER_USE_CRITICAL=1
```

Run the porting implementation on your system:
```c
#include "circular_buffer_porting.h"

/* Porting START */
void circular_buffer_porting_CRITICAL_ENTER(void) {
    // TODO: your implementation
    // Example: pthread_mutex_lock(&mutex);
}

void circular_buffer_porting_CRITICAL_EXIT(void) {
    // TODO: your implementation
    // Example: pthread_mutex_unlock(&mutex);
}
/* Porting END */
```

Example with pthread: `samples/critical_section`