# Memblk - Pseudo-dynamic memory allocation library for C

This project is a custom implementation of dynamic memory allocation for C. It is suitable for embedded systems and compliant with the MISRA C:2012 standard (see paragraph below), that explicitly forbids the use of the standard library functions as `malloc` and `free`.

It doesn't make use of segmentation or paging, but it is based on a pre-allocated empty pool of bytes. The pool is managed by the library, and each cell is virtually allocated or free on request, providing a compatible interface with the standard `malloc`, `calloc`, `realloc` and `free` functions. The virtual allocation is possible thanks to an array of descriptor bytes, one for each cell, adjacent to the pool. The descriptor byte is used to store the status of the cell (allocated or free) and the its role in an allocated block (head, tail, middle or alone). The library also provides a pointer safety check (POSAC) function to verify the validity of a pointer before using it.

## Coding style

- 4 spaces indentation
- open curly braces on the same line of the statement
- all objects are named using the `MBK_` namespace prefix
- after prefix, all constants are in _UPPER\_SNAKE\_CASE_ notation
- after prefix, all functions and typedefs are in _PascalCase_ notation
- after prefix, all variables are in _camelCase_ notation

## MISRA deviations

- Deviation of rule 11.5 _A conversion should not be performed from pointer to void into pointer to object_: The library, to be compatible with the standard library interface, always returns `void` pointers from the allocation functions and the application developer must cast it to a object pointer of the desidered type. Internally, the pool is a static array of `uint8_t` bytes, thus guaranteeing a 1-byte alignment of data. Therefore

## Usage

The library is composed by a single header file `memblk.h` and a single source file `memblk.c`. The function `MBK_Init()` **MUST** be called before allocating pointers.

```c
#include "memblk.h"

int main(void)
{
    // Initialize the library
    MBK_Init();

    // Common stdlib-like usage
    uint32_t *p = MBK_Malloc(sizeof(uint32_t));
    // here, a more complex check can be done to discriminate between different errors
    if (MBK_Posac(p) != MBK_POSAC_OK)
    {
        // MISRA forbids the use of fprintf, so replace it with a custom error handling whenever possible
        fprintf(stderr, "Memory allocation error\n");
        return 1;
    }
    *p = 42U;
    // ...do stuff with pointer...
    MBK_Free(p);
    return 0;
}
```