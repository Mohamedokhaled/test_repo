
#include "dlms_api.h"
#include "sys-utils.h"
#include "BaseTime.h"


#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>


#define SYSAL_MEM_POOL_MAX_CHUNK_COUNT	32U
#define SYSAL_MEM_POOL_NUM_OF_COUNT		21U /* don't forget to increment when adding new buffer*/
#define SYSAL_MEM_POOL_MAGIC			0xDE


typedef struct {
	uint8_t size;
	uint8_t is_avail;
	uint8_t magic;
	uint8_t index;
} mem_chunk_hdr_t;

typedef struct {
	uint8_t *ptr[SYSAL_MEM_POOL_MAX_CHUNK_COUNT];
	uint8_t avail_count;
	uint8_t count;
	uint16_t size;
} mem_pool_t;



static mem_pool_t mem_pool[SYSAL_MEM_POOL_NUM_OF_COUNT];
static uint32_t mem_pool_last = 0;
static uint32_t mem_pool_initialized = 0;

int32_t sysal_mem_pool_init(uint8_t *heap_ptr, const uint32_t heap_size, const uint32_t chunk_size, const uint32_t chunk_count) {
	int32_t ret = 0;
	uint32_t i;
	uint8_t *current_ptr = heap_ptr;
	mem_chunk_hdr_t *chunk_hdr_ptr;
	/*ptrace((chunk_count <= SYSAL_MEM_POOL_MAX_CHUNK_COUNT),
	"memory chunk count %u exceeds the maximum count. %u\n",
	chunk_count, SYSAL_MEM_POOL_MAX_CHUNK_COUNT);

	ptrace((heap_ptr != NULL), "invalid NULL heap address.\n");*/

	for(i = 0; i < chunk_count; i++) {
		/*ptrace(((current_ptr - heap_ptr) < heap_size), "passed heap size can not accommodate the memory pool count/size\n");*/
		mem_pool[mem_pool_last].ptr[i] = current_ptr;
		chunk_hdr_ptr = (mem_chunk_hdr_t*)current_ptr;
		chunk_hdr_ptr->is_avail = 1U;
		chunk_hdr_ptr->size = chunk_size;
		chunk_hdr_ptr->index = mem_pool_last;
		chunk_hdr_ptr->magic = SYSAL_MEM_POOL_MAGIC;
		current_ptr += chunk_size + 4U;
	}
	mem_pool[mem_pool_last].count = chunk_count;
	mem_pool[mem_pool_last].size = chunk_size;
	mem_pool[mem_pool_last].avail_count = chunk_count;
	mem_pool_last ++;
	mem_pool_initialized = 1U;
	return ret;
}

static uint8_t* sysal_mem_pool_find_avail_chunk(mem_pool_t* mem_pool_ptr) {
	uint8_t *ret = NULL;
	uint32_t i;
	mem_chunk_hdr_t *chunk_hdr_ptr;

	if(!mem_pool_initialized) {
		return NULL;
	}

	for(i = 0; i < mem_pool_ptr->count; i++) {
		chunk_hdr_ptr = (mem_chunk_hdr_t *)mem_pool_ptr->ptr[i];
		if(chunk_hdr_ptr->is_avail == 1U) {
			SYSAL_CRITICAL_START();
			chunk_hdr_ptr->is_avail = 0;
			SYSAL_CRITICAL_END();
			ret = mem_pool_ptr->ptr[i] + 4U;
			break;
		}
	}

	return ret;

}

void* sysal_mem_alloc(const uint32_t size) {
	uint32_t i = 0;
	uint8_t* ret = NULL;
	for(i = 0; i < mem_pool_last; i++ ) {
		if(size <= mem_pool[i].size &&
		mem_pool[i].avail_count > 0) {
			ret = sysal_mem_pool_find_avail_chunk(&mem_pool[i]);
			if(ret != NULL) {
				SYSAL_CRITICAL_START();
				mem_pool[i].avail_count --;
				SYSAL_CRITICAL_END();
				break;
			}
		}
	}
	return (void*)ret;
}

void sysal_mem_free(void *ptr) {
	mem_chunk_hdr_t *chunk_hdr_ptr = (mem_chunk_hdr_t *)((uint8_t*)ptr - 4U);

	/*ptrace((chunk_hdr_ptr->magic == SYSAL_MEM_POOL_MAGIC),"trying to free memory doesn't belong to memory pool\n");*/
	if(chunk_hdr_ptr->is_avail == 0) {
		SYSAL_CRITICAL_START();
		chunk_hdr_ptr->is_avail = 1U;
		mem_pool[chunk_hdr_ptr->index].avail_count ++;
		ptr = NULL; /* youssef : to prevent unintentional memory access */
		SYSAL_CRITICAL_END();
	}
}

void* sysal_mem_calloc(uint8_t num , const uint32_t size)
{
	uint32_t i = 0;
	uint8_t* ret = NULL;
	uint32_t lsize = size * num;
	
	for(i = 0; i < mem_pool_last; i++ ) {
		if(lsize <= mem_pool[i].size &&
		mem_pool[i].avail_count > 0) {
			ret = sysal_mem_pool_find_avail_chunk(&mem_pool[i]);
			if(ret != NULL) {
				SYSAL_CRITICAL_START();
				mem_pool[i].avail_count --;
				SYSAL_CRITICAL_END();
				break;
			}
		}
	}
	return (void*)ret;
}
void* sysal_mem_realloc(void *ptr , const uint32_t size)
{
  uint8_t* ret = NULL;
  return (void*)ret;
}
uint32_t sysal_mem_get_needed_size(uint32_t chunk_size, uint32_t chunk_count) {
	return (chunk_size + 4U) * chunk_count;
}

uint32_t sysal_get_current_time(void) {
    /* todo: write implementation for the target system*/
	return 10;
	/*return now;*/
}

uint32_t sysal_get_elapsed_time_ms(void) {
    /* todo: write implementation for the target system */
	
    return time_tick.TO_ms;
}

int32_t dlms_send_frame(uint8_t* frame, uint32_t size) {
	uint8_t ret = 0 ; 
	if (ret == 0)
	{
      return DLMS_SUCCESS;
	}
	else
	{ 
      return DLMS_FAIL;
	}
	
}
uint8_t Buff0[1024];
uint8_t Buff1[1024];
uint8_t Buff2[1024];
uint8_t Buff3[1024];
uint8_t Buff4[1024];
uint8_t Buff5[1024];
uint8_t Buff6[1024];
uint8_t Buff7[1024];
uint8_t Buff8[1024];
uint8_t Buff9[1024];
uint8_t Buff10[1024];
uint8_t Buff11[1024];
uint8_t Buff12[1024];
uint8_t Buff13[1024];
uint8_t Buff14[1024];
uint8_t Buff15[1024];
uint8_t Buff16[1024];
uint8_t Buff17[1024];
uint8_t Buff18[1024];
uint8_t Buff19[1024];
uint8_t Buff20[1024];

void sysal_init (int sizeOfC)
{
	/* before adding new buffer make sure you increment SYSAL_MEM_POOL_NUM_OF_COUNT*/
	
   sysal_mem_pool_init(Buff0,  512 , 32 , 29);
   sysal_mem_pool_init(Buff1,  512 , 32 , 29);
   sysal_mem_pool_init(Buff2,  512 , 32 , 29);
   sysal_mem_pool_init(Buff3,  512 , 32 , 29);
   sysal_mem_pool_init(Buff4,  512 , 32 , 29);
   sysal_mem_pool_init(Buff5,  512 , 32 , 29);
   sysal_mem_pool_init(Buff6,  512 , 32 , 29);
   sysal_mem_pool_init(Buff7,  512 , 32 , 29);
   sysal_mem_pool_init(Buff8,  512 , 32 , 29);
   sysal_mem_pool_init(Buff9,  512 , 32 , 29);
   sysal_mem_pool_init(Buff10, 512 , 32 , 29);
   sysal_mem_pool_init(Buff11, 512 , 32 , 29); 
   sysal_mem_pool_init(Buff12, 512 , 32 , 29); 
   sysal_mem_pool_init(Buff13, 512 , 32 , 29); 
   sysal_mem_pool_init(Buff14, 512 , 32 , 29);
   sysal_mem_pool_init(Buff15, 512 , 32 , 29);
   sysal_mem_pool_init(Buff16, 512 , sizeOfC ,1);
   sysal_mem_pool_init(Buff17, 512 , sizeOfC ,1);
   sysal_mem_pool_init(Buff18, 512 , sizeOfC ,1);
   sysal_mem_pool_init(Buff19, 512 , sizeOfC ,1);
   sysal_mem_pool_init(Buff20, 512 , sizeOfC ,1);

}

