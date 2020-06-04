#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 1024
#define BAD_POPEN "Can't popen"
#define BAD_GETS "Can't gets"
#define BAD_PCLOSE "Can't pclose"
 
int main(int argc, char **argv)
{
 	FILE *fin;

 	char res[BUF_SIZE];
 	int resPClose;

 	if (argc < 2)
    	{
        	printf("Usage: %s file_name\n", argv[0]);
        	exit(1);
    	}

    	char* line = (char*)malloc(BUF_SIZE);

    	strcat(line, "cat ");
    	strcat(line, argv[1]);
    	strcat(line, " | grep '^$' | wc -l");

	fin = popen(line, "r");

	if(fin == NULL)
	{
		perror(BAD_POPEN);
		exit(2);
	}

	fgets(res, BUF_SIZE, fin);
	printf("%d\n", atoi(res));

	resPClose = pclose(fin);

	if(resPClose == -1)
			perror(BAD_PCLOSE);

	exit(0);
}
