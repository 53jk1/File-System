// > gcc overlapped.c -Wall -Wextra -o overlapped
// > overlapped

#include <windows.h>
#include <string.h>
#include <stdio.h>

static char buffer[1024 * 1024 * 128]; // 128MB

int main(void) {
    HANDLE h = CreateFile(
        "overlapped.file",
        GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
        FILE_FLAG_OVERLAPPED, // Marking that asynchronous operations will be performed.
        NULL);

    if (h == INVALID_HANDLE_VALUE) {
        printf("error: failed to open file (%u)\n",
            (unsigned int)GetLastError());
        return 1;
    }

    OVERLAPPED ov;
    memset(&ov, 0, sizeof(ov));
    // Optionally, you can specify an event object that will be signaled when the operation is complete.

    if (!WriteFile(h, buffer, sizeof(buffer), NULL, &ov)) {
        if (GetLastError() == ERROR_IO_PENDING) {
            puts("Asynchronous write scheduled.");
        } else {
            printf("error: scheduling write failed (%u)\n",
                (unsigned int)GetLastError());
            CloseHandle(h);
            return 2;
        }
    }

    printf("Writing: Zz");
    for(;;) {
        DWORD bytes_written = 0;
        BOOL ret = GetOverlappedResult(h, &ov, &bytes_written, FALSE);
        // Alternatively, the HasOverlappedIoCompleted macro could be used.
        if (ret) {
            break;
        }
        printf("z"); fflush(stdout);

        // Do some other very hard work. Possibly fall asleep just pretending to be working.
        Sleep(10);
    }

    puts("\nDone!");
    CloseHandle(h);
    return 0;
}