// > gcc -std=c99 -Wall -Wextra goodguybuffer.c -O3 -o goodguybuffer
// > goodguybuffer.exe

#include <time.h>
#include <stdio.h>
#include <stdbool.h>

void perform_test(const char *fname, bool buffered) {
    FILE *F = fopen(fname, "wb");
    if (!buffered) {
        // Disabling buffering.
        setvbuf(f, NULL, _IONBF, 0);
    }

    // Lots of little records.
    for (int i = 0; i < 1024 * 1024; ++i) {
        char c = 'A';
        fwrite(&c, 1, 1, f);
    }

    fclose(f);
}

int main(void) {
    clock_t a, b, c;

    a = clock();
    perform_test("buffered", true);
    b = clock();
    perform_test("unbuffered", false);
    c = clock();

    printf("Buffered  : %u clocks\n", b - a);
    printf("Unbuffered: %u clocks\n", c -b);

    return 0;
}