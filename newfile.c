// $ gcc newfile.c -Wall -Wextra -o newfile
// $ ./newfile
// $ ls -la /tmp/knownname
// $ ./newfile

#include <fcntl.h>
#include <grp.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    int fd = open("/tmp/knownname",
                    O_CREAT |  // Create a new file if it doesn't exist.
                    O_EXCL | // Make sure you will definitely create
                             // a new file (ie the file did not exist
                             // before).
                    O_WRONLY, // Open for writing only.
                    0600 // S_IRUSR | S_IWUSR - or rw-------
                    );
    
    if (fd == -1) {
        perror("Failed to create file");
        return 1;
    }

    // At this point, only the current user has access to the new file.

    // ...

    // Suppose that after performing the applied operations on the file,
    // we want to give users in the www-data group read-only access to
    // the file.

    struct group *www_data = getgrnam("www-data");
    if (www_data == NULL) {
        perror("Failed to get ID of group www-data");
        close(fd);
        return 2;
    }

    // Note: The user must be a member of the target group to be able to
    // change the group of a file.
    if (fchown(fd, -1, www_data ->gr_gid) == -1) {
        perror("Failed to change group");
        close(fd);
        return 3;
    }

    // New permission: S_IRGRP, which gives rw-r -----.
    if (fchmod(fd, 0640) == -1) {
        perror("Failed to change permissions");
        close(fd);
        return 4;
    }

    close(fd);
    return 0;
}