#include "../src/memblk.h"
#include "unity.h"

void setUp(void) {
    MBK_Init();
}

void tearDown(void) {
    // clean stuff up here
}

void allocate_5_blocks_of_uint32(void) {
    uint32_t *ptr = MBK_Malloc(5U * sizeof(uint32_t));
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_EQUAL(MBK_POSAC_OK, MBK_Posac(ptr));
}

void try_to_allocate_more_than_pool_size(void) {
    TEST_ASSERT_NULL(MBK_Malloc(100001U));
    TEST_ASSERT_EQUAL(MBK_ERR_OVERFLOW, MBK_GetError());
}

void try_to_free_middle_cell(void) {
    uint32_t *ptr = MBK_Malloc(2U * sizeof(uint32_t));
    TEST_ASSERT_NOT_NULL(ptr);
    uint8_t *nextPtr = (uint8_t *)ptr + 1U;
    TEST_ASSERT_EQUAL(MBK_POSAC_NOHEAD, MBK_Posac(nextPtr));
}

void try_to_allocate_struct(void) {
    typedef struct S {
        uint32_t a;
        int16_t b;
    } s;

    s *ptr = MBK_Malloc(5 * sizeof(s));
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_EQUAL(MBK_POSAC_OK, MBK_Posac(ptr));

    for (int16_t i = 0; i < 5; i++) {
        ptr[i].a = i;
        ptr[i].b = -i;
    }

    for (int16_t i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL(i, ptr[i].a);
        TEST_ASSERT_EQUAL(-i, ptr[i].b);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(allocate_5_blocks_of_uint32);
    RUN_TEST(try_to_allocate_more_than_pool_size);
    RUN_TEST(try_to_free_middle_cell);
    RUN_TEST(try_to_allocate_struct);
    return UNITY_END();
}