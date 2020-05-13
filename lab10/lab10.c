#define BAD_FORK "Can't fork"
#define BAD_EXECVP "Can't execl"
#define BAD_WAIT "Can't wait"

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int status = 0;
    pid_t child, ret;

    if (argc < 2)
    {
        printf("Usage: %s file_name\n", argv[0]);
        exit(1);
    }

    child = fork();

    if (child == -1)
    {
        perror(BAD_FORK);
        exit(1);
    }
    else if (child == 0)
    {
        printf("child:\n");
        int checkExecvp = execvp(argv[1], &argv[1]);
        if (checkExecvp == -1)
        {
            perror(BAD_EXECVP);
            exit(1);
        }
        exit(1);
    }

    printf("parent: waiting for child=%ld\n", child);

    ret = wait(&status);
    if (ret == -1)
    {
        perror(BAD_WAIT);
        exit(1);
    }
    printf("parent: return value=%ld\n", ret);

    if (WIFEXITED(status))
    {
        printf("parent: child's exit status is %d\n", WEXITSTATUS(status));
    }

    exit(0);
}
