#include <stdio.h>
#include <stdlib.h>

#define BAD_POPEN "Can't popen"
#define BAD_PUT "Can't put"
#define BAD_CLOSE "Bad close"

int main()
{
    FILE *fptr;
    char *line ={"AbcDEfgHijKLMNopl\n"};

    fptr = popen("./lab26_2.exe", "w");

    if(fptr == NULL)
    {
        perror(BAD_POPEN);
        exit(1);
    }

    int resPuts = fputs(line, fptr);

    int resClose = pclose(fptr);

    if(resClose == -1)
    {
        perror(BAD_CLOSE);
        exit(3);
    }

    if(resPuts == EOF)
    {
        perror(BAD_PUT);
        exit(2);
    }

    exit(0);
 }
