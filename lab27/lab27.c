#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 1024
#define BAD_OPEN "Can't open"
#define BAD_POPEN "Can't popen"
#define BAD_GETS "Can't gets"
#define BAD_PUTS "Can't puts"
#define BAD_PCLOSE "Can't pclose"
#define BAD_FCLOSE "Can't fclose"
 
int main(int argc, char **argv)
{
 	FILE *fin, *fout;
 	int resPClose, resFClose;

 	if (argc < 2)
    {
        printf("Usage: %s file_name\n", argv[0]);
        exit(1);
    }

	char buf[BUF_SIZE];

	fin = fopen(argv[1], "r");

	if(fin == NULL)
	{
		perror(BAD_OPEN);
		exit(2);
	}

	fout = popen("wc -l", "w");

	if(fout == NULL)
	{
		perror(BAD_POPEN);
		resFClose = fclose(fin);

		if(resFClose == EOF)
			perror(BAD_FCLOSE);

		exit(3);
	}

	int resPuts;

	while(fgets(buf, BUF_SIZE, fin) != (char *)NULL)
	{

		if (buf[0] == '\n')
			resPuts = fputs(buf, fout);

		if(resPuts == EOF)
		{
			perror(BAD_PUTS);

			resFClose = fclose(fin);

			if(resFClose == EOF)
				perror(BAD_FCLOSE);

			resPClose = pclose(fout);

			if(resPClose == -1)
				perror(BAD_PCLOSE);

			exit(4);
		}
	}

	resFClose = fclose(fin);

	if(resFClose == EOF)
			perror(BAD_FCLOSE);

	resPClose = pclose(fout);

	if(resPClose == -1)
			perror(BAD_PCLOSE);

	exit(0);
}
