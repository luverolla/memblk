#include "memblk.h"

#include <stdio.h>

static uint8_t *MBK_POOL;
static uintptr_t MBK_POOL_HEAD;
static MBK_Error mbkLastError = MBK_ERR_OK;

/**
 * @brief Get the pool index of the cell pointed by ptr
 * @param ptr pointer to the cell (must be in the pool)
 *
 * Since this function is called by MBK_PtrValid, to avoid call loops, it is assumed
 * that the pointer is safe. Any other routine calling this function MUST check for
 * pointer safety through MBK_PtrValid.
 *
 * Example:
 * <pre>
 * if (MBK_PtrValid(ptr, MBK_BLK_8)) {
 *    size_t idx = MBK_GetPoolIdx(ptr);
 *   ...
 * }
 * </pre>
 */
static size_t MBK_GetPoolIdx(uintptr_t ptr) {
    return (size_t)(ptr - MBK_POOL_HEAD);
}

/**
 * @brief Get the descriptor index of the cell pointed by ptr
 * @param ptr pointer to the cell (must be in the pool)
 *
 * Since this function is called by MBK_PtrValid, to avoid call loops, it is assumed
 * that the pointer is safe. Any other routine calling this function MUST check for
 * pointer safety through MBK_PtrValid.
 *
 * Example:
 * <pre>
 * if (MBK_PtrValid(ptr, MBK_BLK_8)) {
 *    size_t idx = MBK_GetDescIdx(ptr);
 *   ...
 * }
 * </pre>
 */
static size_t MBK_GetDescIdx(uintptr_t ptr) {
    return MBK_GetPoolIdx(ptr) + MBK_POOL_SIZE;
}

/**
 * @brief Check if pointer is valid
 * @param ptr pointer to check
 *
 * Checks if the pointer valid, hence if it is not NULL and it is in the pool limits
 */
static bool MBK_PtrValid(void *ptr) {
    bool ptrValid = false;

    if (ptr != NULL) {
        uintptr_t ptrInt = (uintptr_t)(void *)ptr;
        if ((ptrInt >= MBK_POOL_HEAD) && (ptrInt < (MBK_POOL_HEAD + MBK_POOL_SIZE))) {
            ptrValid = true;
        }
    }

    return ptrValid;
}

/**
 * @brief Get the descriptor of the cell pointed by ptr
 * @param ptr pointer to the cell (must be in the pool)
 *
 * Since this function is called by MBK_PtrValid, to avoid call loops, it is assumed
 * that the pointer is safe. Any other routine calling this function MUST check for
 * pointer safety through MBK_PtrValid.
 *
 * Example:
 * <pre>
 * if (MBK_PtrValid(ptr, MBK_BLK_8)) {
 *    MBK_CellDesc desc = MBK_GetDesc(ptr);
 *   ...
 * }
 * </pre>
 */
static MBK_CellDesc MBK_GetDesc(void *ptr) {
    return MBK_POOL[MBK_GetPoolIdx((uintptr_t)(void *)ptr) + MBK_POOL_SIZE];
}

static uint8_t MBK_Desc_GetFreeFlag(MBK_CellDesc desc) {
    return (desc & 0x04U) >> 2U;
}

static void MBK_Desc_SetFreeFlag(MBK_CellDesc *desc, uint8_t free) {
    *desc = (*desc & 0xFBU) | (free << 2U);
}

static MBK_CellPos MBK_Desc_GetBlockPos(MBK_CellDesc desc) {
    return (desc & 0x18U) >> 3U;
}

static void MBK_Desc_SetBlockPos(MBK_CellDesc *desc, MBK_CellPos pos) {
    *desc = (*desc & 0xE7U) | ((uint8_t)pos << 3U);
}

/**
 * @brief Check if the cell pointed by ptr is "dependent"
 *
 * A cell is dependent if it is part of a block and it is not the head of the block.
 */
static bool MBK_IsCellDependent(void *ptr) {
    MBK_CellDesc desc = MBK_GetDesc(ptr);
    return (MBK_Desc_GetBlockPos(desc) == MBK_POS_MIDDLE) || (MBK_Desc_GetBlockPos(desc) == MBK_POS_TAIL);
}

static bool MBK_IsCellFree(void *ptr) {
    MBK_CellDesc desc = MBK_GetDesc(ptr);
    return MBK_Desc_GetFreeFlag(desc) == 0U;
}

/**
 * @brief Get the first available block of size "size"
 * @param size size of the block to allocate
 * @return pointer to the first available block of size "size" or NULL if no block is available
 *
 * Starting from the beginning of the pool, this function looks for the first available block of given size
 */
static void *MBK_GetAval(size_t size) {
    void *ptr = NULL;
    uint32_t idx = 0U;
    for (; idx < MBK_POOL_SIZE; idx++) {
        bool found = true;
        uint32_t runIdx = idx;
        for (; runIdx < (idx + size); runIdx++) {
            if (!MBK_IsCellFree(&MBK_POOL[runIdx])) {
                found = false;
                break;
            }
        }

        if (found) {
            ptr = &MBK_POOL[idx];
            break;
        } else {
            idx = runIdx + 1U;
        }
    }

    return ptr;
}

void MBK_Init(void) {
    const size_t MBK_TOTAL_SIZE = 2U * (size_t)MBK_POOL_SIZE;
    uint8_t arr[MBK_TOTAL_SIZE];
    MBK_POOL = arr;
    MBK_POOL_HEAD = (uintptr_t)(void *)MBK_POOL;
    for (size_t i = 0U; i < MBK_TOTAL_SIZE; i++) {
        MBK_POOL[i] = 0x00U;
    }
}

void *MBK_Malloc(size_t size) {
    void *ptr = NULL;
    if ((size > 0U) && (size <= MBK_POOL_SIZE)) {
        ptr = MBK_GetAval(size);
        size_t start = MBK_GetPoolIdx((uintptr_t)(void *)ptr);
        for (size_t i = start; i < (start + size); i++) {
            MBK_Desc_SetFreeFlag(&MBK_POOL[i + MBK_POOL_SIZE], 0x01U);
            if (size == 1U) {
                MBK_Desc_SetBlockPos(&MBK_POOL[i + MBK_POOL_SIZE], MBK_POS_LONELY);
            } else if (i == start) {
                MBK_Desc_SetBlockPos(&MBK_POOL[i + MBK_POOL_SIZE], MBK_POS_HEAD);
            } else if (i == (start + size - 1U)) {
                MBK_Desc_SetBlockPos(&MBK_POOL[i + MBK_POOL_SIZE], MBK_POS_TAIL);
            } else {
                MBK_Desc_SetBlockPos(&MBK_POOL[i + MBK_POOL_SIZE], MBK_POS_MIDDLE);
            }
        }
    }

    if (ptr == NULL) {
        mbkLastError = MBK_ERR_OVERFLOW;
    }
    return ptr;
}

void MBK_Free(void *ptr) {
    bool error = true;
    if (MBK_PtrValid(ptr)) {
        size_t start = MBK_GetDescIdx((uintptr_t)(void *)ptr);
        error = false;

        if (!MBK_IsCellDependent(ptr)) {
            if (MBK_Desc_GetBlockPos(MBK_POOL[start]) == MBK_POS_HEAD) {
                size_t idx = start + 1U;
                // both MBK_POS_MIDDLE (0x02) and MBK_POS_TAIL (0x03) are greater than MBK_POS_HEAD (0x01)
                while (MBK_Desc_GetBlockPos(MBK_POOL[idx]) > MBK_POS_HEAD) {
                    MBK_Desc_SetFreeFlag(&MBK_POOL[idx], 0x00U);
                    MBK_Desc_SetBlockPos(&MBK_POOL[idx], MBK_POS_LONELY);
                    idx++;
                }
            }
            MBK_Desc_SetFreeFlag(&MBK_POOL[start], 0x00U);
            MBK_Desc_SetBlockPos(&MBK_POOL[start], MBK_POS_LONELY);
        }
    }

    if (error) {
        mbkLastError = MBK_ERR_PTRINV;
    }
}

void *MBK_Calloc(size_t nmemb, size_t size) {
    void *ptr = MBK_Malloc(nmemb * size);
    if (ptr != NULL) {
        size_t startIdx = MBK_GetPoolIdx((uintptr_t)(void *)ptr);
        for (size_t i = startIdx; i < (startIdx + (nmemb * size)); i++) {
            MBK_POOL[i] = 0U;
        }
    }

    return ptr;
}

void *MBK_Realloc(void *ptr, size_t size) {
    void *newPtr = NULL;
    if (MBK_PtrValid(ptr)) {
        MBK_Free(ptr);
        newPtr = MBK_Malloc(size);
    }

    return newPtr;
}

MBK_PosacOutcome MBK_Posac(void *ptr) {
    MBK_PosacOutcome outcome = MBK_POSAC_NULL;

    if (ptr != NULL) {
        uintptr_t ptrInt = (uintptr_t)(void *)ptr;
        if ((ptrInt >= MBK_POOL_HEAD) && (ptrInt < (MBK_POOL_HEAD + MBK_POOL_SIZE))) {
            if (!MBK_IsCellFree(ptr)) {
                if (MBK_IsCellDependent(ptr)) {
                    outcome = MBK_POSAC_NOHEAD;
                } else {
                    outcome = MBK_POSAC_OK;
                }
            } else {
                outcome = MBK_POSAC_FREED;
            }
        } else {
            outcome = MBK_POSAC_OUTPOOL;
        }
    }

    return outcome;
}

MBK_Error MBK_GetError(void) {
    return mbkLastError;
}

const char *MBK_ReportError(void) {
    static const char *MBK_ERROR_MSGS[] = {
        "OK",
        "Invalid pointer",
        "Location not in pool",
    };
    return MBK_ERROR_MSGS[mbkLastError];
}
