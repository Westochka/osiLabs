#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define BAD_POPEN "Can't popen"
#define BAD_PUT "Can't put"
#define BAD_PCLOSE "Bad pclose"
#define BAD_CHILD "Child process finished with error"

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

    int resPClose = pclose(fptr);

    if(resPClose == -1)
    {
        perror(BAD_PCLOSE);
        exit(2);
    }

    if(WIFEXITED(resPClose) != 0 && WEXITSTATUS(resPClose) != 0)
	{
		printf("Child process finished with status: %d\n", WEXITSTATUS(resPClose));
		exit(3);
	}
	else if(WIFEXITED(resPClose) == 0)
	{
		perror(BAD_CHILD);
		exit(4);
	}

    if(resPuts == EOF)
    {
        perror(BAD_PUT);
        exit(5);
    }

    exit(0);
 }
