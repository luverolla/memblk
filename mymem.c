#include "mymem.h"

#include <stdio.h>

void print_pool()
{
    printf("Pool:\n");
    for (uint32_t i = 0; i < MM_POOL_SIZE; i += 1)
    {
        if (MM_POOL[i] == 0)
            continue;

        if (i % 32 == 0)
            printf("\n");

        if (i % 4 == 0)
            printf(" ");

        printf("%u", MM_POOL[i]);
    }
    printf("\n");
}

void print_stats()
{
    printf("Stats:\n");
    for (uint32_t i = 0; i < MM_STAT_POOL_SIZE; i += 1)
    {
        if (MM_STAT_POOL[i] == 0U)
            continue;

        printf("%u: %u\n", i, MM_STAT_POOL[i]);
    }
    printf("\n");
}

uint8_t _mm_get_pb(MM_Stat stat, uint8_t blk)
{
    return (stat >> (4U * blk)) & 0x1U;
}

uint8_t _mm_get_sb(MM_Stat stat, uint8_t blk)
{
    if (blk == 0U)
        return (stat >> 1U) & 0x3U;
    else
        return (stat >> 5U) & 0x3U;
}

uint8_t _mm_get_cb(MM_Stat stat, uint8_t blk)
{
    if (blk == 0U)
        return (stat >> 3U) & 0x1U;
    else
        return (stat >> 7U) & 0x1U;
}

void _mm_set_pb(MM_Stat *stat, uint8_t blk, uint8_t val)
{
    *stat = (*stat & ~(0x1U << (4U * blk))) | (val << (4U * blk));
}

void _mm_set_sb(MM_Stat *stat, uint8_t blk, uint8_t val)
{
    if (blk == 0U)
        *stat = (*stat & ~(0x3U << 1U)) | (val << 1U);
    else
        *stat = (*stat & ~(0x3U << 5U)) | (val << 5U);
}

void _mm_set_cb(MM_Stat *stat, uint8_t blk, uint8_t val)
{
    *stat = (*stat & ~(0x1U << (4U * blk + 3U))) | (val << (4U * blk + 3U));
}

uint8_t _mm_is_blk_free(MM_Stat stat, uint8_t blk)
{
    return _mm_get_sb(stat, blk) == 0U;
}

uint32_t _mm_get_stat_idx(void *ptr)
{
    return ((uint8_t *)ptr - (uint8_t *)MM_POOL) / 2U;
}

uint8_t _mm_get_stat_blk(void *ptr)
{
    return ((uint8_t *)ptr - (uint8_t *)MM_POOL) % 2U;
}

void _mm_recursive_free(void *ptr, uint8_t is_first_call)
{
    uint32_t stat_idx = _mm_get_stat_idx(ptr);
    MM_Stat stat = MM_STAT_POOL[stat_idx];
    uint8_t blk = _mm_get_stat_blk(ptr);

    if (_mm_is_blk_free(stat, blk))
        return;

    // the block to free belongs to a chain of allocated blocks
    // it cannot be freed without deleting the first block of the chain
    // but, if this is not the first call, the first block has already been freed
    if (_mm_get_pb(stat, blk) && is_first_call)
        return;

    // free the block
    uint8_t *actualPtr = (uint8_t *)ptr;
    uint8_t size = _mm_get_sb(stat, blk) * 4U;
    for (uint8_t i = 0U; i < size; i++)
        actualPtr[i] = 0U;

    // if block continues, free the next block recursively
    if (_mm_get_cb(stat, blk))
        _mm_recursive_free((void *)(actualPtr + size), 0x0U);

    // set size bits to zero
    _mm_set_sb(&MM_STAT_POOL[stat_idx], blk, 0U);
    // set continuation bit to zero
    _mm_set_cb(&MM_STAT_POOL[stat_idx], blk, 0U);
}

void *_mm_recursive_malloc(uint32_t size, uint8_t is_first_call)
{
    // search for free blocks
    uint8_t *cell = MM_POOL;

    for (uint32_t i = 0; i < MM_STAT_POOL_SIZE; i++)
    {
        MM_Stat stat = MM_STAT_POOL[i];

        for (uint8_t blk = 0U; blk < 2U; blk++)
        {
            if (!_mm_is_blk_free(stat, blk))
            {
                // move to next block
                uint8_t blkSize = _mm_get_sb(stat, 0x0U);
                blkSize = blkSize == 3U ? 4U : blkSize;
                cell += blkSize;
                continue;
            }

            uint8_t targetSize = size > 4U ? 4U : size;
            uint32_t remainingSize = size - targetSize;

            // allocate current block
            uint8_t sizeByte = size > 3U ? 3U : size;
            _mm_set_sb(&MM_STAT_POOL[i], blk, sizeByte);
            _mm_set_pb(&MM_STAT_POOL[i], blk, is_first_call ? 0x00U : 0x01U);
            _mm_set_cb(&MM_STAT_POOL[i], blk, remainingSize > 0U ? 0x01U : 0x00U);

            // allocate further blocks recursively
            if (remainingSize > 0)
                _mm_recursive_malloc(remainingSize, 0U);

            // return pointer to allocated block
            return (void *)cell;
        }
    }

    // no free block found
    return NULL;
}

void *mm_malloc(uint32_t size)
{
    void *ptr = _mm_recursive_malloc(size, 1U);
    return ptr;
}

void *mm_calloc(uint32_t num, uint32_t size)
{
    void *ptr = mm_malloc(num * size);
    if (ptr == NULL)
        return NULL;

    uint8_t *actualPtr = (uint8_t *)ptr;
    for (uint32_t i = 0U; i < num * size; i++)
        actualPtr[i] = 0U;

    return ptr;
}

void *mm_realloc(void *ptr, uint32_t size)
{
    if (ptr == NULL)
        return mm_malloc(size);

    uint32_t stat_idx = _mm_get_stat_idx(ptr);
    MM_Stat stat = MM_STAT_POOL[stat_idx];

    // if the block is not the first of a chain, it cannot be resized
    if (_mm_get_pb(stat, 0x0U))
        return NULL;

    uint8_t blk = _mm_get_stat_blk(ptr);

    uint8_t oldSize = _mm_get_sb(stat, blk) * 4U;
    uint8_t newSize = size > 4U ? 4U : size;

    if (oldSize == newSize)
        return ptr;

    if (oldSize > newSize)
    {
        // shrink
        uint8_t *actualPtr = (uint8_t *)ptr;
        for (uint8_t i = newSize; i < oldSize; i++)
            actualPtr[i] = 0U;

        _mm_set_sb(&MM_STAT_POOL[stat_idx], blk, newSize / 4U);
        _mm_set_cb(&MM_STAT_POOL[stat_idx], blk, 0U);
        return ptr;
    }
    else
    {
        // expand
        uint8_t *actualPtr = (uint8_t *)ptr;
        for (uint8_t i = oldSize; i < newSize; i++)
            actualPtr[i] = 0U;

        uint32_t remainingSize = size - newSize;
        _mm_set_sb(&MM_STAT_POOL[stat_idx], blk, newSize / 4U);
        _mm_set_cb(&MM_STAT_POOL[stat_idx], blk, remainingSize > 0U ? 0x01U : 0x00U);

        if (remainingSize > 0)
            _mm_recursive_malloc(remainingSize, 0U);

        return ptr;
    }
}

void mm_free(void *ptr)
{
    _mm_recursive_free(ptr, 1U);
}