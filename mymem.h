/**
 * @file mymem.h
 * @brief Memory allocator
 *
 * High efficiency and low space consuming memory allocator for embedded systems
 * Given P the size of the memory pool, the allocator uses 1.5P total bytes of memory
 *  - P bytes for the memory pool
 *  - P/2 bytes for the memory pool status
 */

#ifndef MYMEM_H
#define MYMEM_H

#include <stdlib.h>
#include <stdint.h>

#define MM_POOL_SIZE 100000U
#define MM_STAT_POOL_SIZE (MM_POOL_SIZE / 2U)

/**
 * @brief Memory block status
 * Contains status about two adjacent memory blocks B1 and B2
 * It is divided in two 4-bit parts, one for each block
 *
 * [PB1 SB1[1] SB1[0] CB1 | PB2 SB2[1] SB2[0] CB2]
 *
 * For each part
 * bit 0 is MSB, bit 3 is LSB
 * ProsecutionBit - PB[0] = block is (1) or not (0) the continuation of a previous block
 * SizeBits - SB[1:2] = number of bytes occupied by the block. A block of size zero is considered as free
 * ContinuationBit - CB[3] = block continues (1) or not (0) in the next block
 */
typedef uint8_t MM_Stat;

static uint8_t MM_POOL[MM_POOL_SIZE];
static MM_Stat MM_STAT_POOL[MM_STAT_POOL_SIZE] = {0U};

void *mm_malloc(uint32_t size);
void *mm_calloc(uint32_t num, uint32_t size);
void *mm_realloc(void *ptr, uint32_t size);
uint8_t mm_is_ptr_safe(void *ptr);
void mm_free(void *ptr);

#endif