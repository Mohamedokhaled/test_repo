#include <stdint.h>
#include <stdarg.h>
#include "sys-utils.h"


#ifndef __LINUX__
void* malloc(const uint32_t size) {
	return (void*)sysal_mem_alloc(size);
}

void free(void* ptr) {
	sysal_mem_free(ptr);
}
#endif /* __LINUX__ */

uint32_t time_elapsed(void)
{
#ifdef __LINUX__
    return (uint32_t)(clock() / (CLOCKS_PER_SEC / (clock_t)1000U));
#else
    return sysal_get_elapsed_time_ms();
#endif /* __LINUX__ */
}

uint32_t time_current(void) {
#ifdef __LINUX__
    return (uint32_t)time(NULL);
#else
	return sysal_get_current_time();
#endif /* __LINUX__ */
}

void sysal_trace(const char *fmt, ...) {
#ifdef __LINUX__
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
#else
    /* todo: write implementation for the target system */
#endif /* __LINUX__ */
}
