
#ifndef CIRCULAR_BUFFER_DEFS_H
#define CIRCULAR_BUFFER_DEFS_H

#include <stdbool.h>
#include <stdint.h>

#ifndef cb_size_t
typedef uint16_t cb_size_t;
#endif

#define CB_MAX_ELEMENT_LEN (0xFFF)

enum circular_buffer_status_e {
    CIRCULAR_BUFFER_SUCCESS = 0,
    CIRCULAR_BUFFER_INVALID_PARAM = 1,
    CIRCULAR_BUFFER_INSUFFICIENT_SPACE = 2,
};

typedef uint8_t circular_buffer_status_t;

struct circular_buffer_s {
    volatile cb_size_t head;
    volatile cb_size_t tail;
    struct {
        uint8_t *ptr;
        cb_size_t size;
        bool overwrite_oldest;
        uint8_t element_len;
    } buffer;
};

typedef struct circular_buffer_s circular_buffer_t;

#define CB_BUFFER_PTR(cb) (cb->buffer.ptr)
#define CB_BUFFER_SIZE(cb) (cb->buffer.size)
#define CB_OVERWRITE_OLDEST(cb) (cb->buffer.overwrite_oldest)
#define CB_ELEMENT_LEN(cb) (cb->buffer.element_len)
#define CB_HEAD(cb) (cb->head)
#define CB_TAIL(cb) (cb->tail)

#define CB_HEADER_ID (0x9) /* Header Id - 4 bits */
#define CB_HEADER_SIZE (0x2)
#define CB_HEAD_GET_HEADER(cb)                                                 \
    (CB_BUFFER_PTR(cb)[CB_HEAD(cb)] << 8 |                                     \
     CB_BUFFER_PTR(cb)[(CB_HEAD(cb) + 1) % CB_BUFFER_SIZE(cb)])
#define CB_HEADER_GET_DATA_LEN(cb)                                             \
    ((CB_BUFFER_PTR(cb)[CB_HEAD(cb)] & 0x0F) |                                 \
     CB_BUFFER_PTR(cb)[(CB_HEAD(cb) + 1) % CB_BUFFER_SIZE(cb)])
#define CB_HEADER_INSERT(cb, len)                                              \
    ({                                                                         \
        CB_BUFFER_PTR(cb)                                                      \
        [CB_TAIL(cb)] = (uint8_t)((CB_HEADER_ID << 4) | len >> 8);             \
        CB_BUFFER_PTR(cb)                                                      \
        [(CB_TAIL(cb) + 1) % CB_BUFFER_SIZE(cb)] = (uint8_t)len;               \
    })
#define CB_GET_TAIL_FORWARD_SPACE(cb)                                          \
    {                                                                          \
        CB_HEAD(cb) <= CB_TAIL(cb) ? (CB_BUFFER_SIZE(cb) - CB_TAIL(cb))        \
                                   : (CB_HEAD(cb) - CB_TAIL(cb))               \
    }
#define CB_GET_HEAD_FORWARD_LEN(cb)                                            \
    {                                                                          \
        CB_HEAD(cb) < CB_TAIL(cb) ? (CB_TAIL(cb) - CB_HEAD(cb))                \
                                  : (CB_BUFFER_SIZE(cb) - CB_HEAD(cb))         \
    }

#endif /* CIRCULAR_BUFFER_DEFS_H */