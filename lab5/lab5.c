#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define BAD_OPEN "Can't open file"
#define BAD_LINE "Can't read line\n"
#define BAD_ARGUMENTS "Don't found name of file"
#define BAD_MEMORY "Can't allocate memory"
#define MAX_SIZE 1024

int main(int argc, char **argv)
{
    int fin, lineNumber, currentLengthLine = 0, currentLine = 1, *lengths, *carryOvers;
    lengths = (int*)calloc(MAX_SIZE, sizeof(int));
    carryOvers = (int*)calloc(MAX_SIZE, sizeof(int));
    char simbol;

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

    carryOvers[currentLine] = 0;

    while(read(fin, &simbol, 1))
    {
        if(simbol == '\n')
        {
            lengths[currentLine++] = currentLengthLine;
            carryOvers[currentLine] = lseek(fin, 0, 1);
            currentLengthLine = 0;
        }
        else
            currentLengthLine++;
    }
    
    while(scanf("%d", &lineNumber))
    {
        if(lineNumber <= 0)
        {
            free(lengths);
            free(carryOvers);
            close(fin);
            exit(1);
        }
        if(!carryOvers[lineNumber] && !lengths[lineNumber] && lineNumber != 1)
        {
            write(1, BAD_LINE, strlen(BAD_LINE));
            continue;
        }
        lseek(fin, carryOvers[lineNumber], 0);

        char* textLine = (char*)malloc(lengths[lineNumber] + 1);

        if(textLine == NULL)
        {
            free(lengths);
            free(carryOvers);
            close(fin);
            perror(BAD_MEMORY);
            exit(1);
        }

        if(read(fin, textLine, lengths[lineNumber] + 1))
            write(1, textLine, lengths[lineNumber] + 1);

        free(textLine);
    }
    free(lengths);
    free(carryOvers);
    close(fin);
    return 0;
}
