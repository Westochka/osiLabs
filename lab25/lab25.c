#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define BUF_SIZE 1024
#define BAD_PIPE "Can't pipe!"
#define BAD_FORK "Can't fork!"
#define BAD_WRITE "Can't write"
#define BAD_READ "Can't read"
#define BAD_WAIT "Can't wait"

int nonIntrRead(int fd, void* buf, size_t count)
{
    while (1)
    {
        int ret = read(fd, buf, count);
        if (ret == -1 && errno == EINTR)
            continue;
        return ret;
    }
}

int nonIntrWrite(int fd, void* buf, size_t count)
{
    while(1)
    {
        int wr = write(fd, buf, count);
        if(wr == -1 && errno == EINTR)
            continue;
        return wr;
    }
}

int main(int argc, char** argv)
{
        pid_t pid1, pid2;
        int fd[2];
        char* line ={ "AbcDEfgHijKLMNop\n"};
        char buf[BUF_SIZE];

        if (pipe(fd) == -1)
        {
                perror(BAD_PIPE);
                exit(1);
 }

        pid1 = fork();
        if (pid1 == 0)
        {
                close(fd[0]);
                int resWrite = nonIntrWrite(fd[1], line, strlen(line));
                if(resWrite == -1)
                {
                        close(fd[1]);
                        perror(BAD_WRITE);
                        exit(3);
                }
                close(fd[1]);
                exit(0);
        }
        else if(pid1 == -1)
        {
                close(fd[1]);
                close(fd[0]);
                perror(BAD_FORK);
                exit(4);
        }

        pid2 = fork();
        if (pid2 == 0)
        {
                close(fd[1]);
                int resRead = nonIntrRead(fd[0], buf, BUF_SIZE);
                if(resRead == -1)
                {
                        close(fd[0]);
                        perror(BAD_READ);
                        exit(5);
                }
                for(int i = 0; i < resRead; i++)
                        if(islower(buf[i]))
                                buf[i] = toupper(buf[i]);
                int resWrite = nonIntrWrite(1, buf, resRead);
                if(resWrite == -1)
                {
                        close(fd[0]);
                        perror(BAD_WRITE);
                        exit(6);
                }
                close(fd[0]);
                exit(0);
 }
        else if(pid2 == -1)
        {
                close(fd[1]);
                close(fd[0]);
                perror(BAD_FORK);
                exit(2);
        }

        close(fd[0]);
        close(fd[1]);

        int waitRes1, waitRes2, status1 = 0, status2 = 0;

        waitRes1 = waitpid(pid1, &status1, NULL);
        if(waitRes1 == -1)
                perror(BAD_WAIT);

        waitRes2 = waitpid(pid2, &status2, NULL);
        if(waitRes2 == -1)
                perror(BAD_WAIT);

        exit(0);
}
