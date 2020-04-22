#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/mman.h>

#define BAD_OPEN "Can't open file"
#define BAD_LINE "Can't read line\n"
#define BAD_MEMORY "Can't allocate memory"
#define GET_LINE_FOR_FIVE_SEC "Please, type number of line for 5 seconds\n"
#define BAD_LSEEK "Can't lseek"
#define BAD_CLOSE "Can't close file"
#define BAD_WRITE "Can't write file"
#define BAD_READ_LINE "Can't read line"
#define BAD_WRITE_LINE "Can't write line"
#define BAD_POLL "Can't poll"
#define BAD_MMAP "Can't mmap"
#define MAX_SIZE 1024

int nonIntrRead(int fd, void* buf, size_t count)
{
    while (1)
    {
        int ret = read(fd, buf, count);
        if (ret == -1 && errno == EINTR)
            continue;
        return ret;
    }
}

int nonIntrWrite(int fd, void* buf, size_t count)
{
    while(1)
    {
        int wr = write(fd, buf, count);
        if(wr == -1 && errno == EINTR)
            continue;
        return wr;
    }
}

int readingBuff(off_t fileSize, char** carryOvers, int *lengths, char* p)
{
    int currentLine = 1, currentLengthLine = 0;
    carryOvers[currentLine] = p;

    for (off_t i = 0; i < fileSize; i++)
    {
        if (*(p + i) == '\n')
        {
            lengths[currentLine++] = currentLengthLine;
            carryOvers[currentLine] = p + i + 1;
            currentLengthLine = 0;
        }
        else
            currentLengthLine++;
    }
    return currentLine;
}

int scanningLines(int lines, int* lengths, char** carryOvers, char* p, int fileSize)
{
    struct pollfd fds;
    fds.fd = STDIN_FILENO;
    fds.events = POLLIN;

    int lineNumber;

    while (1)
    {
        write(1, GET_LINE_FOR_FIVE_SEC, strlen(GET_LINE_FOR_FIVE_SEC));
        int pollAnsw = poll(&fds, 1, 5000);

        if (pollAnsw == 0)
        {
            int resWrite = nonIntrWrite(1, p, fileSize);
            if(resWrite == -1)
                return 2;
            return 1;
        }
        else if (pollAnsw == -1)
            return 3;
        else
        {
            scanf("%d", &lineNumber);

            if (lineNumber <= 0)
                return 1;
            if (lineNumber > lines)
            {
                write(1, BAD_LINE, strlen(BAD_LINE));
                continue;
            }

            int resWrite = nonIntrWrite(1, carryOvers[lineNumber], lengths[lineNumber] + 1);
            if(resWrite == -1)
                return 4;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s file_name\n", argv[0]);
        exit(1);
    }

    int fin = open(argv[1], O_RDONLY);

    if (fin == -1)
    {
        perror(BAD_OPEN);
        exit(1);
    }

    int checkClose;
    off_t fileSize = lseek(fin, 0, SEEK_END);
    lseek(fin, 0, SEEK_SET);

    int *lengths = (int*) calloc(MAX_SIZE, sizeof(int));

    if (lengths == NULL)
    {
        perror(BAD_MEMORY);
        checkClose = close(fin);
        if (checkClose == -1)
            perror(BAD_CLOSE);
        exit(1);
    }

    char** carryOvers = (char**) malloc(MAX_SIZE);

    if (carryOvers == NULL)
    {
        free(lengths);
        perror(BAD_MEMORY);
        checkClose = close(fin);
        if (checkClose == -1)
            perror(BAD_CLOSE);
        exit(1);
    }

    char* p = mmap(0, fileSize, PROT_READ, MAP_SHARED, fin, 0);

    checkClose = close(fin);
    if (checkClose == -1)
    {
        perror(BAD_CLOSE);
        exit(1);
    }

    if(p == NULL)
    {
        free(lengths);
        free(carryOvers);
        perror(BAD_MMAP);
    }

    int lines = readingBuff(fileSize, carryOvers, lengths, p);

    int resScan = scanningLines(lines, lengths, carryOvers, p, fileSize);

    free(lengths);
    free(carryOvers);

   if(resScan == 2)
   {
        perror(BAD_WRITE);
        exit(1);
   }
   else if(resScan == 3)
   {
        perror(BAD_POLL);
        exit(1);
   }
   else if(resScan == 4)
   {
       perror(BAD_WRITE_LINE);
       exit(1);
   }
    exit(0);
}
