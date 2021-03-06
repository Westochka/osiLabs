#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#define BAD_OPEN "Can't open file"
#define BAD_LINE "Can't read line\n"
#define BAD_MEMORY "Can't allocate memory"
#define BAD_LSEEK "Can't lseek"
#define BAD_CLOSE "Can't close file"
#define BAD_READ "Can't read file"
#define BAD_READ_LINE "Can't read line"
#define BAD_WRITE_LINE "Can't write line"
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

int readingBuff(char *buffer, off_t fileSize, int *carryOvers, int *lengths)
{
    int currentCarryOvers = 0, currentLine = 1, currentLengthLine = 0;
    carryOvers[currentLine] = 0;

    for (off_t i = 0; i < fileSize; i++)
    {
        if (buffer[i] == '\n')
        {
            lengths[currentLine++] = currentLengthLine;
            currentCarryOvers += currentLengthLine + 1;
            carryOvers[currentLine] = currentCarryOvers;
            currentLengthLine = 0;
        }
        else
            currentLengthLine++;
    }
    return currentLine;
}

int scanningLines(int lines, int *lengths, int *carryOvers, int fin)
{
    int lineNumber;

    while (scanf("%d", &lineNumber))
    {
        if (lineNumber <= 0)
            return 1;
        if (lineNumber > lines)
        {
            write(1, BAD_LINE, strlen(BAD_LINE));
            continue;
        }
        if (lseek(fin, carryOvers[lineNumber], 0) == -1)
            return 2;

        char *textLine = (char *) malloc(lengths[lineNumber] + 1);

        if (textLine == NULL)
            return 3;

        int resRead = nonIntrRead(fin, textLine, lengths[lineNumber] + 1);
        if(resRead == -1)
        {
            free(textLine);
            return 4;
        }

        int resWrite = nonIntrWrite(1, textLine, lengths[lineNumber] + 1);
        if(resWrite == -1)
        {
            free(textLine);
            return 5;
        }

        free(textLine);
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

    char *buffer = (char *) malloc(sizeof(char) * fileSize);

    if (buffer == NULL)
    {
        perror(BAD_MEMORY);
        checkClose = close(fin);
        if (checkClose == -1)
            perror(BAD_CLOSE);
        exit(1);
    }

    int checkReading = nonIntrRead(fin, buffer, fileSize);
    if(checkReading == -1)
    {
        free(buffer);
        perror(BAD_READ);
        checkClose = close(fin);
        if (checkClose == -1)
            perror(BAD_CLOSE);
        exit(1);
    }

    int *lengths = (int*) calloc(MAX_SIZE, sizeof(int));

    if (lengths == NULL)
    {
        free(buffer);
        perror(BAD_MEMORY);
        checkClose = close(fin);
        if (checkClose == -1)
            perror(BAD_CLOSE);
        exit(1);
    }

    int *carryOvers = (int*) calloc(MAX_SIZE, sizeof(int));

    if (carryOvers == NULL)
    {
        free(buffer);
        free(lengths);
        perror(BAD_MEMORY);
        checkClose = close(fin);
        if (checkClose == -1)
            perror(BAD_CLOSE);
        exit(1);
    }

    int lines = readingBuff(buffer, fileSize, carryOvers, lengths);

    free(buffer);

    int resScan = scanningLines(lines, lengths, carryOvers, fin);

    free(lengths);
    free(carryOvers);

    checkClose = close(fin);
    if (checkClose == -1)
    {
        perror(BAD_CLOSE);
        exit(1);
    }

    if (resScan == 2)
    {
        perror(BAD_LSEEK);
        exit(1);
    }
    else if (resScan == 3)
    {
        perror(BAD_MEMORY);
        exit(1);
    }
    else if (resScan == 4)
    {
        perror(BAD_READ_LINE);
        exit(1);
    }
    else if (resScan == 5)
    {
        perror(BAD_WRITE_LINE);
        exit(1);
    }
    exit(0);
}
