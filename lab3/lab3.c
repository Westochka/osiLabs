#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
     FILE *fp;
     uid_t uid;

     if(argc < 2)
     {
         printf("To use program: %s fileName\n", argv[0]);
         return 1;
     }

     printf("at first uid=%ld and euid=%ld\n", getuid(), geteuid());

     if ((fp = fopen(argv[1], "r")) == NULL)
     {
         perror(argv[0]);
         return 1;
     }
     else
     {
         printf("first open was successful\n");
         fclose(fp);
     }

     setuid(uid=getuid());

     printf("after setuid(%ld):\n   uid=%ld and euid=%ld\n", uid, getuid(), geteuid());
     if ((fp = fopen(argv[1], "r")) == NULL)
     {
         perror(argv[0]);
         return 1;
     }
     else
     {
         printf("second open successful\n");
         fclose(fp);
     }
 }
