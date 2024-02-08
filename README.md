# Memblk - Pseudo-dynamic memory allocation library for C

> **Status**: All functions implemented. Project under testing.

This project is a custom implementation of dynamic memory allocation for C. It is suitable for embedded systems and compliant with the MISRA C:2012 standard (see paragraph below), that explicitly forbids the use of the standard library functions as `malloc` and `free`.

It doesn't make use of segmentation or paging, but it is based on a pre-allocated empty pool of bytes. The pool is managed by the library, and each cell is virtually allocated or free on request, providing a compatible interface with the standard `malloc`, `calloc`, `realloc` and `free` functions. The virtual allocation is possible thanks to an array of descriptor bytes, one for each cell, adjacent to the pool. The descriptor byte is used to store the status of the cell (allocated or free) and the its role in an allocated block (head, tail, middle or alone). The library also provides a pointer safety check (POSAC) function to verify the validity of a pointer before using it.

## MISRA deviations

- Deviation of rule 21.6 _The Standard Library input/output functions shall not be used_: The Memblk library exploits the standard I/O library only for the `fprintf` function and only to write string messages to standard error output. This is done to provide error messages to the user in case of memory allocation errors. Error messages are strings contrained by the provided values and the function `MBK_ThrowError` is declared as `static` and thus not accessible for the application developer. Moreover, after an error is thrown and printed, the program is terminated, so any undefined behavior is avoided.