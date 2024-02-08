#include <stdio.h>

#include "memblk.h"

typedef struct SmallType {
    uint8_t a;
    uint8_t b;
} SmallType;

typedef struct MediumType {
    uint8_t eight;
    uint16_t sixteen;
    uint8_t eight2;
} MediumType;

typedef struct LargeType {
    uint8_t eight;
    uint16_t sixteen;
    uint32_t thirtytwo;
    uint8_t eight2;
} LargeType;

int main(int argc, char **argv) {
    (int)argc;
    (char **)argv;

    MBK_Init();

    // trying to allocate more memory than available in the pool
    // void *ptr = MBK_Malloc(MBK_POOL_SIZE + 100U);

    uint8_t *ptr8 = MBK_Malloc(4U * sizeof(uint8_t));
    if (ptr8 == NULL) {
        printf("Error: ptr8 is NULL\n");
    } else {
        for (uint32_t i = 0U; i < 4U; i++) {
            ptr8[i] = (uint8_t)i;
        }
        for (uint32_t i = 0U; i < 4U; i++) {
            printf("ptr8[%u] = %u\n", i, ptr8[i]);
        }
    }

    printf("\n");

    uint32_t *ptr32 = MBK_Malloc(4U * sizeof(uint32_t));
    if (ptr32 == NULL) {
        printf("Error: ptr32 is NULL\n");
    } else {
        for (uint32_t i = 0U; i < 4U; i++) {
            ptr32[i] = i * i;
        }
        for (uint32_t i = 0U; i < 4U; i++) {
            printf("ptr32[%u] = %u\n", i, ptr32[i]);
        }
    }

    printf("\n");

    SmallType *ptrSmall = MBK_Malloc(4 * sizeof(SmallType));
    if (ptrSmall == NULL) {
        printf("Error: ptrSmall is NULL\n");
    } else {
        for (uint32_t i = 0U; i < 4U; i++) {
            ptrSmall[i].a = (uint8_t)i;
            ptrSmall[i].b = (uint8_t)(i + 1U);
        }
        for (uint32_t i = 0U; i < 4U; i++) {
            printf("ptrSmall[%u].a = %u, ptrSmall[%u].b = %u\n", i, ptrSmall[i].a, i, ptrSmall[i].b);
        }
    }

    printf("\n");

    MediumType *ptrMedium = MBK_Malloc(4 * sizeof(MediumType));
    if (ptrMedium == NULL) {
        printf("Error: ptrMedium is NULL\n");
    } else {
        for (uint32_t i = 0U; i < 4U; i++) {
            ptrMedium[i].eight = (uint8_t)i;
            ptrMedium[i].sixteen = (uint16_t)(i + 1U);
            ptrMedium[i].eight2 = (uint8_t)(i + 2U);
        }
        for (uint32_t i = 0U; i < 4U; i++) {
            printf("ptrMedium[%u].eight = %u, ptrMedium[%u].sixteen = %u, ptrMedium[%u].eight2 = %u\n", i, ptrMedium[i].eight, i, ptrMedium[i].sixteen, i, ptrMedium[i].eight2);
        }
    }

    printf("\n");

    LargeType *ptrLarge = MBK_Malloc(4U * sizeof(LargeType));
    if (ptrLarge == NULL) {
        printf("Error: ptrLarge is NULL\n");
    } else {
        for (uint32_t i = 0U; i < 4U; i++) {
            ptrLarge[i].eight = (uint8_t)i;
            ptrLarge[i].sixteen = (uint16_t)(i + 1U);
            ptrLarge[i].thirtytwo = (uint32_t)(i + 2U);
            ptrLarge[i].eight2 = (uint8_t)(i + 3U);
        }
        for (uint32_t i = 0U; i < 4U; i++) {
            printf("ptrLarge[%u].eight = %u, ptrLarge[%u].sixteen = %u, ptrLarge[%u].thirtytwo = %u, ptrLarge[%u].eight2 = %u\n", i, ptrLarge[i].eight, i, ptrLarge[i].sixteen, i, ptrLarge[i].thirtytwo, i, ptrLarge[i].eight2);
        }
    }

    // trying to free a null pointer
    MBK_Free(NULL);
}