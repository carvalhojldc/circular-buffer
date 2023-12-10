#ifndef CIRCULAR_BUFFER_PORTING_H
#define CIRCULAR_BUFFER_PORTING_H

/**
 * @brief Porting critical section
 *        Enter the critical section
 */
void circular_buffer_porting_CRITICAL_ENTER(void);

/**
 * @brief Porting critical section
 *        Exit the critical section
 */
void circular_buffer_porting_CRITICAL_EXIT(void);

#endif /* CIRCULAR_BUFFER_PORTING_H */
