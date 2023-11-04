# Circular Buffer

A simple circular buffer to be used with fixed and dynamic data length.

## Usage

### Initailization

A storage buffer must be allocated and passed at Circular Buffer initialization.

The Circular Buffer will be with:
* dynamic data length: element_len must be 0
* fixed data length: element_line must be the size of the element

```c
#include "circular_buffer.h"

circular_buffer_t cb;
uint8_t storage_buffer[200];
circular_buffer_status_t status;

status = circular_buffer_init(&cb,
                                storage_buffer,
                                sizeof(storage_buffer),
                                true,           /* true -> overwrite_oldest */
                                0);             /* 0    -> element_len (dynamic length) */

char *str = "Hello";
status = circular_buffer_push_dl(&cb, str, strlen(str));

```

### Examples
Available in the `samples` folder.
