#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 1024
#define BAD_READ "Can't read"
#define BAD_WRITE "Can't write"

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

int main()
{
        char buf[BUF_SIZE];

        int resRead = nonIntrRead(0, buf, BUF_SIZE);

        if(resRead == -1)
        {
                perror(BAD_READ);
                exit(1);
        }

        for(int i = 0; i < resRead; i++)
            if(islower(buf[i]))
                    buf[i] = toupper(buf[i]);
            
        int resWrite = nonIntrWrite(1, buf, resRead);

        if(resWrite == -1)
        {
            perror(BAD_WRITE);
            exit(2);
        }

        exit(0);
}
