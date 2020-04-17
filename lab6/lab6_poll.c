#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>

#define BAD_OPEN_FILE "Can't open file"
#define BAD_LINE "Can't read line\n"
#define BAD_OPEN_TERM "Can't open /dev/tty"
#define BAD_ARGUMENTS "Don't found name of file"
#define GET_LINE_FOR_FIVE_SEC "Please, type number of line for 5 seconds\n"
#define BAD_MEMORY "Can't allocate memory"
#define BAD_LSEEK "Can't lseek"
#define BAD_POLL "Can't poll"
#define MAX_SIZE 1024

int openingDiscriptors(int* fin1, int* fin2, char** argv)
{
    if ((*fin1 = open("/dev/tty", O_RDONLY | O_NDELAY)) == -1)
        return 1;

    if ((*fin2 = open(argv[1], O_RDONLY)) == -1)
        return 2;

    return 0;
}

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

int scanningLines(int fin1, int fin2, char* buff, int* lengths, int* carryOvers, int temp, int lineNumber)
{
    struct pollfd fds;
    fds.fd = STDIN_FILENO;
    fds.events = POLLIN;

    int pollAnsw;

    while (1)
    {
        write(1, GET_LINE_FOR_FIVE_SEC, strlen(GET_LINE_FOR_FIVE_SEC));

        pollAnsw =  poll(&fds, 1, 5000);

        if (pollAnsw == 0)
        {
            lseek(fin2, SEEK_SET, 0);
            while ((temp = read(fin2, buff, MAX_SIZE)) > 0)
                write(1, buff, temp);
            return 0;
        } else if(pollAnsw == -1)
            return 3;
        else
        {
            scanf("%d", &lineNumber);

            if(lineNumber <= 0)
                return 0;

            if (!carryOvers[lineNumber] && !lengths[lineNumber] && lineNumber != 1)
            {
                write(1, BAD_LINE, strlen(BAD_LINE));
                continue;
            }
            if(lseek(fin2, carryOvers[lineNumber], 0) == -1)
                return 1;

            char *textLine = (char *) malloc(lengths[lineNumber] + 1);

            if(textLine == NULL)
                return 2;

            if (read(fin2, textLine, lengths[lineNumber] + 1))
                write(1, textLine, lengths[lineNumber] + 1);

            free(textLine);
        }
    }
}

int main(int argc, char **argv)
{
    int fin1, fin2, lineNumber, currentLengthLine = 0, currentLine = 1, *lengths, *carryOvers, temp;
    lengths = (int *) calloc(MAX_SIZE, sizeof(int));

    if(lengths == NULL)
    {
        perror(BAD_MEMORY);
        exit(1);
    }

    carryOvers = (int *) calloc(MAX_SIZE, sizeof(int));

    if(carryOvers == NULL)
    {
        free(lengths);
        perror(BAD_MEMORY);
        exit(1);
    }

    char simbol, *buff;
    buff = (char *) malloc(MAX_SIZE);

    if(buff == NULL)
    {
        free(lengths);
        free(carryOvers);
        perror(BAD_MEMORY);
        exit(1);
    }

    if (argc < 2)
    {
        free(lengths);
        free(carryOvers);
        free(buff);
        perror(BAD_ARGUMENTS);
        exit(1);
    }

    int openD = openingDiscriptors(&fin1, &fin2, argv);
    if(openD == 1)
    {
        free(lengths);
        free(carryOvers);
        free(buff);
        perror(BAD_OPEN_TERM);
        exit(1);
    }
    else if(openD == 2)
    {
        free(lengths);
        free(carryOvers);
        free(buff);
        perror(BAD_OPEN_FILE);
        exit(1);
    }

    if(readingFile(carryOvers, lengths, currentLine, currentLengthLine, fin2) == -1)
    {
        free(lengths);
        free(carryOvers);
        perror(BAD_LSEEK);
        close(fin2);
        exit(1);
    }

    int scanL = scanningLines(fin1, fin2, buff, lengths, carryOvers, temp, lineNumber);
    if(scanL == 1)
    {
        free(buff);
        free(lengths);
        free(carryOvers);
        close(fin2);
        close(fin1);
        perror(BAD_LSEEK);
        exit(1);
    }
    else if(scanL == 2)
    {
        free(buff);
        free(lengths);
        free(carryOvers);
        close(fin2);
        close(fin1);
        perror(BAD_MEMORY);
        exit(1);
    }
    else if(scanL == 3)
    {
        free(buff);
        free(lengths);
        free(carryOvers);
        close(fin2);
        close(fin1);
        perror(BAD_POLL);
        exit(1);
    }

    free(buff);
    free(lengths);
    free(carryOvers);
    close(fin2);
    close(fin1);
    return 0;
}
