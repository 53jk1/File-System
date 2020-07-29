// > cl /W4 goodguykernelcache.c

#include <malloc.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
// MinGW GCC may not have function declarations of the _aligned_* family.

#ifndef _CRT_ALLOCATION_DEFINED
void * _aligned_malloc(
    size_t size,
    size_t alignment
);

void _aligned_free (
    void *memblock
);
#endif

// On the other hand, Visual C++ may not have the stdbool.h header file
// as this was added in C99 for which Visual C++ support is negligible.

typedef int bool;
#define false 0
#define true 1

#define SECTOR_SIZE 4096
static char *buffer;

void perform_test(const char *fname, bool buffered) {
    HANDLE h = CreateFile(
        fname, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
        buffered ? FILE_ATTRIBUTE_NORMAL
                 : FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH,
        NULL);

    // Lots of records.
    static char buffer[SECTOR_SIZE];
    int i;
    for (i = 0; i < 1024 * 16; ++i) {
        DWORD written;
        WriteFile(h, buffer, SECTOR_SIZE, &written, NULL);
    }
    CloseHandle(h)
}

int main(void) {
    clock_t a, b, c;

    // When writing without caching on the kernel, the data must be
    // prepared in a buffer with a size that is a multiple of the size
    // of a single sector on the disk (4096 satisfies this property for
    // both older and newer disks). It is also recommended to locate them
    // on an address aligned with the sector size.
    buffer = _aligned_malloc(SECTOR_SIZE, SECTOR_SIZE);
    memset(buffer, 0, SECTOR_SIZE);

    a = clock();
    perform_test("buffered", true);
    b = clock();
    perform_test("unbuffered", false);
    c = clock();

    printf("Cached  : %u clocks\n", b - a);
    printf("Uncached: %u clocks\n", c - b);

    _aligned_free(buffer);

    return 0;
}