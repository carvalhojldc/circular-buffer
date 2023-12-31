#include "circular_buffer.h"
#include "circular_buffer_critical_section.h"
#include <string.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define CB_HEAD_GET_HEADER(cb)                                                 \
    (CB_BUFFER_PTR(cb)[CB_HEAD(cb)] << 8 |                                     \
     CB_BUFFER_PTR(cb)[(CB_HEAD(cb) + 1) % CB_BUFFER_SIZE(cb)])

#define CB_HEADER_GET_DATA_LEN(cb)                                             \
    (CB_DYNAMIC_LEN(cb)                                                        \
         ? (((CB_BUFFER_PTR(cb)[CB_HEAD(cb)] & 0x0F) << 8) |                   \
            CB_BUFFER_PTR(cb)[(CB_HEAD(cb) + 1) % CB_BUFFER_SIZE(cb)])         \
         : CB_ELEMENT_LEN(cb))

#define CB_HEADER_GET_ID(cb) ((CB_BUFFER_PTR(cb)[CB_HEAD(cb)] & 0xF0) >> 0x4)

#define CB_HEADER_INSERT(cb, len)                                              \
    ({                                                                         \
        CB_BUFFER_PTR(cb)                                                      \
        [CB_TAIL(cb)] = (uint8_t)((CB_HEADER_ID << 4) | len >> 8);             \
        CB_BUFFER_PTR(cb)                                                      \
        [(CB_TAIL(cb) + 1) % CB_BUFFER_SIZE(cb)] = (uint8_t)len;               \
    })

#define CB_GET_TAIL_FORWARD_SPACE(cb)                                          \
    ({                                                                         \
        CB_HEAD(cb) <= CB_TAIL(cb)                                             \
            ? (CB_BUFFER_SIZE(cb) - CB_TAIL(cb))                               \
            : (CB_OVERWRITE_OLDEST(cb) ? (CB_BUFFER_SIZE(cb) - CB_TAIL(cb))    \
                                       : CB_HEAD(cb) - CB_TAIL(cb));           \
    })

#define CB_GET_HEAD_FORWARD_LEN(cb)                                            \
    ({                                                                         \
        CB_HEAD(cb) < CB_TAIL(cb) ? (CB_TAIL(cb) - CB_HEAD(cb))                \
                                  : (CB_BUFFER_SIZE(cb) - CB_HEAD(cb));        \
    })

#define CB_GET_NEXT_HEAD(cb)                                                   \
    ((CB_HEAD(cb) + CB_HEADER_GET_DATA_LEN(cb) + CB_HEADER_SZ(cb)) %           \
     CB_BUFFER_SIZE(cb))

/*******************************************************************************
 * Private prototype
 ******************************************************************************/

static circular_buffer_status_t cb_push(circular_buffer_t *cb, void *data,
                                        cb_size_t len);
static cb_size_t get_empty_space(circular_buffer_t *cb);
static void execute_head_rotation(circular_buffer_t *cb, cb_size_t next_tail);

/*******************************************************************************
 * Public
 ******************************************************************************/

circular_buffer_status_t circular_buffer_init(circular_buffer_t *cb,
                                              uint8_t *buffer, cb_size_t size,
                                              bool overwrite_oldest,
                                              uint8_t element_len) {
    if (cb == NULL || buffer == NULL || size == 0) {
        return CIRCULAR_BUFFER_INVALID_PARAM;
    }

    memset(buffer, 0, size);
    memset(cb, 0, sizeof(circular_buffer_t));

    cb->buffer.ptr = buffer;
    cb->buffer.size = size;
    cb->buffer.overwrite_oldest = overwrite_oldest;
    cb->buffer.element_len = element_len;

    cb->internal.dynamic_len = element_len == 0;
    cb->internal.header_sz = element_len == 0 ? CB_HEADER_SIZE : 0;

    cb->head = 0;
    cb->tail = 0;

    return CIRCULAR_BUFFER_SUCCESS;
}

bool circular_buffer_is_empty(circular_buffer_t *cb) {
    return (cb != NULL ? CB_HEAD(cb) == CB_TAIL(cb) : true);
}

bool circular_buffer_is_full(circular_buffer_t *cb) {
    return (cb != NULL ? CB_HEAD(cb) == (CB_TAIL(cb) + 1) % CB_BUFFER_SIZE(cb)
                       : true);
}

bool circular_buffer_has_enough_space(circular_buffer_t *cb, cb_size_t len) {
    if (cb == NULL)
        return false;

    len = (CB_DYNAMIC_LEN(cb)) ? (len + CB_HEADER_SIZE) : len;

    if (CB_OVERWRITE_OLDEST(cb))
        return ((CB_BUFFER_SIZE(cb) - 1) >= len);
    else
        return (get_empty_space(cb) >= len);
}

circular_buffer_status_t circular_buffer_push(circular_buffer_t *cb,
                                              void *data) {
    return cb_push(cb, data, 0);
}

circular_buffer_status_t circular_buffer_push_dl(circular_buffer_t *cb,
                                                 void *data, cb_size_t len) {
    return cb_push(cb, data, len);
}

circular_buffer_status_t circular_buffer_pop(circular_buffer_t *cb,
                                             uint8_t *o_buffer,
                                             const cb_size_t buffer_size,
                                             cb_size_t *o_element_len) {
    uint8_t *bptr = NULL;
    cb_size_t element_len, head, forward_len;
    circular_buffer_status_t ret;

    *o_element_len = 0;

    if (cb == NULL || o_buffer == NULL || buffer_size == 0)
        return CIRCULAR_BUFFER_INVALID_PARAM;

    if (circular_buffer_is_empty(cb))
        return CIRCULAR_BUFFER_SUCCESS;

    CRITICAL_ENTER;

    head = CB_HEAD(cb);
    forward_len = CB_GET_HEAD_FORWARD_LEN(cb);

    if (CB_DYNAMIC_LEN(cb)) {
        if (CB_HEADER_GET_ID(cb) == CB_HEADER_ID) {
            forward_len = forward_len <= CB_HEADER_SIZE ? 0 : forward_len - 2;
            head = (head + CB_HEADER_SIZE) % CB_BUFFER_SIZE(cb);
        } else {
            CRITICAL_EXIT;
            return CIRCULAR_BUFFER_INVALID_HEADER;
        }
    }

    element_len =
        CB_DYNAMIC_LEN(cb) ? CB_HEADER_GET_DATA_LEN(cb) : CB_ELEMENT_LEN(cb);

    if (buffer_size >= element_len) {
        bptr = CB_BUFFER_PTR(cb) + head;

        if (forward_len >= element_len) {
            memcpy(o_buffer, bptr, element_len);
        } else {
            memcpy(o_buffer, bptr, forward_len);
            memcpy(o_buffer + forward_len, CB_BUFFER_PTR(cb),
                   element_len - forward_len);
        }

        CB_HEAD(cb) = CB_GET_NEXT_HEAD(cb);
        *o_element_len = element_len;

        ret = CIRCULAR_BUFFER_SUCCESS;
    } else {
        ret = CIRCULAR_BUFFER_INSUFFICIENT_SPACE;
    }

    CRITICAL_EXIT;

    return ret;
}

/*******************************************************************************
 * Private
 ******************************************************************************/

static circular_buffer_status_t cb_push(circular_buffer_t *cb, void *data,
                                        cb_size_t len) {
    uint8_t *bptr = NULL;

    if (cb == NULL || data == NULL || (CB_ELEMENT_LEN(cb) == 0 && len == 0))
        return CIRCULAR_BUFFER_INVALID_PARAM;

    const cb_size_t element_len = CB_DYNAMIC_LEN(cb) ? len : CB_ELEMENT_LEN(cb);
    cb_size_t forward_space = CB_GET_TAIL_FORWARD_SPACE(cb);
    cb_size_t tail = CB_TAIL(cb);

    if (!circular_buffer_has_enough_space(cb, element_len))
        return CIRCULAR_BUFFER_INSUFFICIENT_SPACE;

    if (CB_DYNAMIC_LEN(cb)) {
        tail = (tail + CB_HEADER_SIZE) % CB_BUFFER_SIZE(cb);
        forward_space = (forward_space <= CB_HEADER_SIZE)
                            ? (CB_BUFFER_SIZE(cb) - CB_HEADER_SIZE + tail)
                            : forward_space - CB_HEADER_SIZE;
    }
    bptr = CB_BUFFER_PTR(cb) + tail;
    tail = (tail + element_len) %
           CB_BUFFER_SIZE(cb); /* Calculate new tail position */

    execute_head_rotation(cb, tail);

    if (forward_space >= element_len) {
        memcpy(bptr, data, element_len);
    } else {
        const cb_size_t shift_len = element_len - forward_space;
        memcpy(bptr, data, forward_space);
        memcpy(CB_BUFFER_PTR(cb), (uint8_t *)data + forward_space, shift_len);
    }

    if (CB_DYNAMIC_LEN(cb)) {
        /**
         * @brief Insert header: 2 BYTES (CB_HEADER_SIZE)
         *          *  4 bits - header id (CB_HEADER_ID)
         *          * 12 bits - data len
         */
        CB_HEADER_INSERT(cb, element_len);
    }
    CB_TAIL(cb) = tail; /* Set new tail position */

    return CIRCULAR_BUFFER_SUCCESS;
}

static cb_size_t get_empty_space(circular_buffer_t *cb) {
    cb_size_t len;
    if (circular_buffer_is_empty(cb))
        len = CB_BUFFER_SIZE(cb);
    else if (CB_TAIL(cb) < CB_HEAD(cb))
        len = CB_HEAD(cb) - CB_TAIL(cb);
    else
        len = ((CB_BUFFER_SIZE(cb) - CB_TAIL(cb)) + CB_HEAD(cb));
    /**
     * @brief 1 byte is reserved for Tail to be different from Head when the
     * buffer is full */
    return (len - 1);
}

static void execute_head_rotation(circular_buffer_t *cb, cb_size_t next_tail) {
    if (CB_OVERWRITE_OLDEST(cb) && !circular_buffer_is_empty(cb)) {

        CRITICAL_ENTER;

        /* When we have data rotation */
        if (next_tail < CB_TAIL(cb)) {
            /* The NEW tail can overwrite the head during rotation */
            if (CB_TAIL(cb) > CB_HEAD(cb)) {
                while (CB_HEAD(cb) < next_tail) {
                    CB_HEAD(cb) = CB_GET_NEXT_HEAD(cb);
                }
            } else {
                /* The head WILL be overwritten during the tail rotation and
                 * the head must be moved above the NRE tail */
                cb_size_t c_head = CB_HEAD(cb);
                /* Ensures head rotation and head is above the new tail */
                while (c_head <= CB_HEAD(cb) || CB_HEAD(cb) < next_tail) {
                    CB_HEAD(cb) = CB_GET_NEXT_HEAD(cb);
                }
            }
        } else {
            /* When a new element will overwrite the reference of the head
             */
            if (CB_TAIL(cb) < CB_HEAD(cb) && CB_HEAD(cb) < next_tail) {
                cb_size_t c_head = CB_HEAD(cb);
                while (CB_HEAD(cb) < next_tail && c_head <= CB_HEAD(cb)) {
                    CB_HEAD(cb) = CB_GET_NEXT_HEAD(cb);
                }
            }
        }

        if (next_tail == CB_HEAD(cb)) {
            CB_HEAD(cb) = CB_GET_NEXT_HEAD(cb);
        }

        CRITICAL_EXIT;
    }
}