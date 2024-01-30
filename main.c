#include <stdio.h>

#include "mymem.h"

int main()
{

    // allocate 4 bytes treating them as array of 8-bit integers
    // so we have 4 elements
    uint8_t *four_as_one_ptr = (uint8_t *)mm_malloc(4U);

    // allocate 4 bytes treating them as array of 32-bit integers
    // so we have 1 element
    uint32_t *four_as_four_ptr = (uint32_t *)mm_malloc(4U);

    uint32_t *fifty_uint32_ptr = (uint32_t *)mm_malloc(50U * sizeof(uint32_t));

    *four_as_four_ptr = 24U;

    four_as_one_ptr[0] = 1U;
    four_as_one_ptr[1] = 2U;
    four_as_one_ptr[2] = 3U;
    four_as_one_ptr[3] = 4U;

    for (uint32_t i = 0; i < 50U; i += 1)
    {
        fifty_uint32_ptr[i] = 100U + i;
    }

    printf("four_as_one_ptr[0] = %u\n", four_as_one_ptr[0]);
    printf("four_as_one_ptr[1] = %u\n", four_as_one_ptr[1]);
    printf("four_as_one_ptr[2] = %u\n", four_as_one_ptr[2]);
    printf("four_as_one_ptr[3] = %u\n", four_as_one_ptr[3]);
    printf("four_as_four_ptr = %u\n", *four_as_four_ptr);

    printf("\n\n");
    for (uint32_t i = 0; i < 50U; i += 1)
    {
        printf("fifty_uint32_ptr[%u] = %u\n", i, fifty_uint32_ptr[i]);
    }
    return 0;
}