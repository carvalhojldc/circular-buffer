#ifndef CIRCULAR_BUFFER_CRITICAL_SECTION_H
#define CIRCULAR_BUFFER_CRITICAL_SECTION_H

#include "circular_buffer.h"
#include "circular_buffer_porting.h"

#if CIRCULAR_BUFFER_USE_CRITICAL
#define CRITICAL_ENTER                                                         \
    if (CB_OVERWRITE_OLDEST(cb))                                               \
    circular_buffer_ENTER_CRITICAL()

#define CRITICAL_EXIT                                                          \
    if (CB_OVERWRITE_OLDEST(cb))                                               \
    circular_buffer_EXIT_CRITICAL()
#else
#define CRITICAL_ENTER                                                         \
    {} /* disabled */
#define CRITICAL_EXIT                                                          \
    {} /* disabled */
#endif

#endif /* CIRCULAR_BUFFER_CRITICAL_SECTION_H */
