#ifndef _SYS_UTILS_H_
#define _SYS_UTILS_H_

#include <stdint.h>

/* Define entering critical section by disable interrupts */
#define SYSAL_CRITICAL_START()
#define SYSAL_CRITICAL_END()

void sysal_init (int sizeOfC);
int32_t 	sysal_mem_pool_init(uint8_t *heap_ptr, const uint32_t heap_size,
				const uint32_t chunk_size, const uint32_t chunk_count);
uint32_t 	sysal_mem_get_needed_size(uint32_t chunk_size, uint32_t chunk_count);
void* 		sysal_mem_alloc(const uint32_t size);
void 		sysal_mem_free(void *ptr);
void*       sysal_mem_calloc(uint8_t num , const uint32_t size);
void*       sysal_mem_realloc(void *ptr , const uint32_t size);

uint32_t 	sysal_get_current_time(void);
uint32_t 	sysal_get_elapsed_time_ms(void);

#endif /* _SYS_UTILS_H_ */
