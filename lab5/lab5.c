#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define BAD_OPEN "Can't open file"
#define BAD_LINE "Can't read line\n"
#define BAD_ARGUMENTS "Don't found name of file"
#define BAD_MEMORY "Can't allocate memory"
#define BAD_LSEEK "Can't lseek"
#define MAX_SIZE 1024

int readingFile(int* carryOvers, int* lengths, int currentLine, int currentLengthLine, int fin)
{
    char simbol;
    carryOvers[currentLine] = 0;

    while(read(fin, &simbol, 1))
    {
        if(simbol == '\n')
        {
            int ls;
            if((ls = lseek(fin, 0, 1)) == -1)
                return -1;
            lengths[currentLine++] = currentLengthLine;
            carryOvers[currentLine] = ls;
            currentLengthLine = 0;
        }
        else
            currentLengthLine++;
    }
    return 0;
}

int scanningLines(int lineNumber, int* lengths, int* carryOvers, int fin)
{
    while(scanf("%d", &lineNumber))
    {
        if(lineNumber <= 0)
            return 1;
        if(!carryOvers[lineNumber] && !lengths[lineNumber] && lineNumber != 1)
        {
            write(1, BAD_LINE, strlen(BAD_LINE));
            continue;
        }
        if(lseek(fin, carryOvers[lineNumber], 0) == -1)
            return 2;

        char* textLine = (char*)malloc(lengths[lineNumber] + 1);

        if(textLine == NULL)
            return 3;

        if(read(fin, textLine, lengths[lineNumber] + 1))
            write(1, textLine, lengths[lineNumber] + 1);

        free(textLine);
    }
}

int main(int argc, char **argv)
{
    int fin, lineNumber, currentLengthLine = 0, currentLine = 1, *lengths, *carryOvers;
    lengths = (int*)calloc(MAX_SIZE, sizeof(int));

    if(lengths == NULL)
    {
        perror(BAD_MEMORY);
        exit(1);
    }

    carryOvers = (int*)calloc(MAX_SIZE, sizeof(int));

    if(carryOvers == NULL)
    {
        free(lengths);
        perror(BAD_MEMORY);
        exit(1);
    }

    if(argc < 2)
    {
        free(lengths);
        free(carryOvers);
        perror(BAD_ARGUMENTS);
        exit(1);
    }

    if ((fin = open(argv[1], O_RDONLY)) == -1)
    {
        free(lengths);
        free(carryOvers);
        perror(BAD_OPEN);
        exit(1);
    }

    if(readingFile(carryOvers, lengths, currentLine, currentLengthLine, fin) == -1)
    {
        free(lengths);
        free(carryOvers);
        perror(BAD_LSEEK);
        close(fin);
        exit(1);
    }

    int resScan = scanningLines(lineNumber, lengths, carryOvers, fin);

    if(resScan == 1)
    {
        free(lengths);
        free(carryOvers);
        close(fin);
        exit(1);
    }
    else if(resScan == 2)
    {
        free(lengths);
        free(carryOvers);
        perror(BAD_LSEEK);
        close(fin);
        exit(1);
    }
    else if(resScan == 3)
    {
        free(lengths);
        free(carryOvers);
        close(fin);
        perror(BAD_MEMORY);
        exit(1);
    }

    return 0;
}
