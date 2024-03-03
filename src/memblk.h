/**
 * @file mymem.h
 * @brief Memory allocator
 *
 * High efficiency and low space consuming memory allocator for embedded systems
 * Given P the size of the memory pool, the allocator uses 1.5P total bytes of memory
 *  - P bytes for the memory pool
 *  - P/2 bytes for the memory pool status
 */

#ifndef MEMBLK_H
#define MEMBLK_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Memory Pool Size (in bytes)
 *
 * This parameter is customizable by the application developer.
 */
#define MBK_POOL_SIZE 100000U

/**
 * @brief CEll Descriptor
 * Contains description of a memory cell. It is so structured:
 * [7:6] - CellPosition: encode the position of the cell in a block
 *  00 - lonely cell (no other cell in the block)
 *  01 - head cell (first cell in block)
 *  10 - middle block (neither alone nor head nor tail)
 *  11 - tail block (last cell in block)
 * [5] - FreeFlag: 0 if the cell is free, 1 if it is allocated
 * [4:0] - Reserved for future use
 */
typedef uint8_t MBK_CellDesc;

typedef enum MBK_Error {
    MBK_ERR_OK = 0U,       // no error
    MBK_ERR_PTRINV = 1U,   // pointer is not valid
    MBK_ERR_OVERFLOW = 2U, // memory pool overflow
} MBK_Error;

/**
 * @brief Position of a cell in a memory block
 */
typedef enum MBK_CellPos {
    MBK_POS_LONELY = 0U, // cell is alone in the block
    MBK_POS_HEAD = 1U,   // cell is the head of the block
    MBK_POS_MIDDLE = 2U, // cell is neither head nor tail
    MBK_POS_TAIL = 3U,   // cell is the tail of the block
} MBK_CellPos;

/**
 * @brief Possible outcomes of MBK_Posac
 */
typedef enum MBK_PosacOutcome {
    MBK_POSAC_OK = 0U,      // pointer is valid and can be used or freed
    MBK_POSAC_NULL = 1U,    // pointer is not valid because it is NULL
    MBK_POSAC_FREED = 2U,   // pointer is not valid because it is not allocated
    MBK_POSAC_OUTPOOL = 3U, // pointer is not valid because it is out of pool
    MBK_POSAC_NOHEAD = 4U,  // pointer is valid but it is not the head of the block (it is not safe to free it)
} MBK_PosacOutcome;

void *MBK_Malloc(size_t size);
void MBK_Init(void);
void MBK_Free(void *ptr);
void *MBK_Realloc(void *ptr, size_t size);
void *MBK_Calloc(size_t nmemb, size_t size);
MBK_PosacOutcome MBK_Posac(void *ptr);
MBK_Error MBK_GetError(void);
const char *MBK_ReportError(void);
void MBK_Dump();

#endif