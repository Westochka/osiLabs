#define _CRT_SECURE_NO_WARNINGS
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#define BAD_FILE "Can't open file"
#define BUSY_FILE "Can't open file, because file captured"

int main(int argc, char** argv)
{
    struct flock lock;
    char* command;
    int fin;
    if((fin = open(argv[1], O_RDWR)) == -1)
    {
        perror(BAD_FILE);
        exit(1);
    }

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fin, F_SETLK, &lock) == -1)
    {
        perror(BUSY_FILE);
        exit(2);
    }

    sprintf(command, "vim %s\n", argv[1]);
    system(command);

    lock.l_type = F_UNLCK;
    fcntl(fin, F_SETLK, &lock);

    close(fin);
    exit(0);
}
