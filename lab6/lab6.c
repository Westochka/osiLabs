#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define BAD_OPEN_FILE "Can't open file"
#define BAD_LINE "Can't read line\n"
#define BAD_OPEN_TERM "Can't open /dev/tty"
#define BAD_ARGUMENTS "Don't found name of file"
#define GET_LINE_FOR_FIVE_SEC "Please, type number of line for 5 seconds\n"
#define MAX_SIZE 1024

int main(int argc, char **argv)
{
    int fin1, fin2, lineNumber, currentLengthLine = 0, currentLine = 1, *lengths, *carryOvers, temp;
    lengths = (int *) calloc(MAX_SIZE, sizeof(int));
    carryOvers = (int *) calloc(MAX_SIZE, sizeof(int));
    char simbol, *buff;
    buff = (char *) malloc(MAX_SIZE);

    if (argc < 2)
    {
        perror(BAD_ARGUMENTS);
        exit(1);
    }

    if ((fin1 = open("/dev/tty", O_RDONLY | O_NDELAY)) == -1)
    {
        perror(BAD_OPEN_TERM);
        exit(1);
    }

    if ((fin2 = open(argv[1], O_RDONLY)) == -1)
    {
        perror(BAD_OPEN_FILE);
        exit(1);
    }

    carryOvers[currentLine] = 0;

    while (read(fin2, &simbol, 1))
    {
        if (simbol == '\n')
        {
            lengths[currentLine++] = currentLengthLine;
            carryOvers[currentLine] = lseek(fin2, 0, 1);
            currentLengthLine = 0;
        } else
            currentLengthLine++;
    }


    while (1)
    {
        write(1, GET_LINE_FOR_FIVE_SEC, strlen(GET_LINE_FOR_FIVE_SEC));
        sleep(5);

        if ((temp = read(fin1, buff, MAX_SIZE)) == 0)
        {
            lseek(fin2, SEEK_SET, 0);
            while ((temp = read(fin2, buff, MAX_SIZE)) > 0)
                write(1, buff, temp);
            free(buff);
            free(lengths);
            free(carryOvers);
            close(fin2);
            close(fin1);
            exit(0);
        } else
        {
            lineNumber = atoi(buff);
            if (!carryOvers[lineNumber] && !lengths[lineNumber] && lineNumber != 1)
            {
                write(1, BAD_LINE, strlen(BAD_LINE));
                continue;
            }
            lseek(fin2, carryOvers[lineNumber], 0);

            char *textLine = (char *) malloc(lengths[lineNumber] + 1);
            if (read(fin2, textLine, lengths[lineNumber] + 1))
                write(1, textLine, lengths[lineNumber] + 1);

            free(textLine);
        }
    }
}
