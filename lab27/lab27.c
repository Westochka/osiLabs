#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define LINE_SIZE 128
#define BUF_SIZE 1024
#define BAD_POPEN "Can't popen"
#define BAD_PCLOSE "Bad pclose"
 
int main(int argc, char **argv)
{
 	FILE *fin;

 	int answ;
 	int resPClose;

 	if (argc < 2)
    {
        printf("Usage: %s file_name\n", argv[0]);
        exit(1);
    }

    char* line = (char*)malloc(LINE_SIZE);

    strcat(line, "cat ");
    strcat(line, argv[1]);
    strcat(line, " | grep '^$' | wc -l");

	fin = popen(line, "r");

	free(line);

	if(fin == NULL)
	{
		perror(BAD_POPEN);
		exit(2);
	}

	fscanf(fin, "%d", &answ);

	resPClose = pclose(fin);

	if(resPClose != -1)
	{
			if(WIFEXITED(resPClose) == 0 && WEXITSTATUS(resPClose) != 0)
			{
					perror(BAD_PCLOSE);
					exit(3);
			}
	}

	printf("%d\n", answ);

	exit(0);
}
