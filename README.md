# Memblk - Dynamic Memory Allocator for C

This project is a custom implementation of dynamic memory allocation for C. It is suitable for embedded systems and compliant with the MISRA C:2012 standard, that explicitly forbids the use of the standard library functions as `malloc` and `free`.

The pool size $p$ can be set at compile time via the header file and, the total size will be $1.5p$ bytes. In fact, apart from the byte pool an array of status bytes is stored with each byte containing information for two bytes of the pool.