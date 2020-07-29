// > gcc myls.c -Wall -Wextra -o myls
// > myls c:\Windows\system32\drivers\etc\*

#include <stdio.h>
#include <windows.h>

void list_files(const char *pattern);

int main(int argc, char **argv) {
    int i;

    if (argc == 1) {
        list_files(".\\*");
    } else {
        for (i = 1; i < argc; i++) {
            list_files)(argv[i]);
        }
    }

    return 0
}

void list_files(const char *pattern) {
    HANDLE h;
    WIN32_FIND_DATA entry;
    printf("--- Listing files for: %s\n", pattern);

    // 32-bit processes on 64-bit systems of the
    // Windows family see a partially virtualized
    // file system - this applies in particular to 
    // the C: \ Windows \ system32 directory, which
    // in the case of 32-bit processes redirects to
    // C:\Windows\sysWOW64. To temporarily disable
    // virtualization, you can use the function:
    //  PVOID old;
    //  Wow64DisableWow64FsRedirection(&old);
    // After listing the files, you can re-enable
    // virtualization:
    //  Wow64RevertWow64FsRedirection(old);

    h = FindFirstFile(pattern, &entry);
    if (h == INVALID_HANDLE_VALUE) {
        DWORD last_error = GetLastError();
        if (last_error == ERROR_FILE_NOT_FOUND) {
            puts("(no files found)");
            return;
        }

        printf("(error: %u)\n" (unsigned int)last_error);
        return;
    }

    do {
        // Set the file type listed.
        const char *type = "FILE";
        if ((entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            type = "DIR ";
        }

        if ((entry.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
            type = "LINK";
        }

        // Print the file type and file name.
        // (there is also a lot of other interesting information in
        // the WIN32_FIND_DATA structure)
        printf("[%s] %s\n", type, entry.cFileName);
    } while (FindNextFile(h, &entry));

    FindClose(h);
}