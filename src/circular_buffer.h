#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include "circular_buffer_defs.h"

/**
 * @brief Initialize the Circular Buffer
 *
 * @param [in] cb Pointer of a Circular Buffer object
 * @param [in] buffer Pointer to an allocated Buffer to be used by the Circular
 * Buffer
 * @param [in] size Buffer size (1 byte will be reserved to control the index)
 * @param [in] overwrite_oldest Overwrite oldest data when buffer is full (ring
 * insertion)
 * @param [in] element_len Length of each element to be save, use 0 for Dynamic
 * Length
 *
 * @return circular_buffer_status_t (CIRCULAR_BUFFER_SUCCESS,
 * CIRCULAR_BUFFER_INVALID_PARAM)
 */
circular_buffer_status_t circular_buffer_init(circular_buffer_t *cb,
                                              uint8_t *buffer, cb_size_t size,
                                              bool overwrite_oldest,
                                              uint8_t element_len);

/**
 * @brief Checks if the Circular Vuffer is empty
 *
 * @param [in] cb Circular Buffer pointer
 *
 * @return True if is empty, false if not
 */
bool circular_buffer_is_empty(circular_buffer_t *cb);

/**
 * @brief Checks whether the circular buffer is full
 *
 * @param [in] cb Circular Buffer pointer
 *
 * @return True if is full, false if not
 */
bool circular_buffer_is_full(circular_buffer_t *cb);

/**
 * @brief Checks whether the Circular Buffer can accept more N bytes
 *
 * @param [in] cb Circular Buffer pointer
 * @param [in] len N bytes
 *
 * @return True if can can, false if not
 */
bool circular_buffer_has_enough_space(circular_buffer_t *cb, cb_size_t len);

/**
 * @brief Inserts data into the Circular Buffer
 *
 * @param [in] cb Circular Buffer pointer
 * @param [in] data Pointer to the Data to be inserted into the Circular Buffer
 *
 * @return circular_buffer_status_t (CIRCULAR_BUFFER_INVALID_PARAM,
 * CIRCULAR_BUFFER_INSUFFICIENT_SPACE, CIRCULAR_BUFFER_SUCCESS)
 */
circular_buffer_status_t circular_buffer_push(circular_buffer_t *cb,
                                              void *data);

/**
 * @brief Inserts data with Dynamic Length into the Circular Buffer
 *
 * @param [in] cb Circular Buffer pointer
 * @param [in] data Pointer to the Data to be inserted into the Circular Buffer
 * @param [in] len Data len in bytes
 *
 * @return circular_buffer_status_t (CIRCULAR_BUFFER_INVALID_PARAM,
 * CIRCULAR_BUFFER_INSUFFICIENT_SPACE, CIRCULAR_BUFFER_SUCCESS)
 */
circular_buffer_status_t circular_buffer_push_dl(circular_buffer_t *cb,
                                                 void *data, cb_size_t len);

/**
 * @brief Read data from the Circular Buffer
 *
 * @param [in] cb Circular Buffer pointer
 * @param [out] o_buffer Pointer to the buffer used to save the read data
 * @param [in] size Buffer size in bytes
 * @param [out] o_element_len Number of read bytes
 *
 * @return circular_buffer_status_t
 */
circular_buffer_status_t circular_buffer_pop(circular_buffer_t *cb,
                                             uint8_t *o_buffer,
                                             const cb_size_t size,
                                             cb_size_t *o_element_len);

#endif /* CIRCULAR_BUFFER_H */