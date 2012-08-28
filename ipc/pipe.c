#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>

#define BUFSIZE 10000
#define BUFFLEN strlen(wbuf) + 1

int main()
{
    int pipefd[2], r_num, w_num;
    char rbuf[BUFSIZE];
    char wbuf[BUFSIZE];
    pid_t pid;

    memset(rbuf, 0, BUFSIZE);
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return -1;
    }

    if ((pid = fork()) == 0)
    {
        printf("CHILD: %d\n", getpid());
        close(pipefd[1]);
        sleep(5);

        while (1) {
            r_num = read(pipefd[0], rbuf, BUFSIZE);
            if (!r_num)
                break;
            printf("CHILD: read %d bytes: %s\n", r_num, rbuf);
            sleep(1);
        }
        close(pipefd[0]);
        printf("CHILD: close read\n");
        return 0;
    } else if (pid > 0) {
        close(pipefd[0]);
        
        while (fgets(wbuf, BUFSIZE, stdin)) {
            if (!strcmp(wbuf, "exit\n"))
                break;
            wbuf[strlen(wbuf)-1] = '\0';
            w_num = write(pipefd[1], wbuf, BUFSIZE);
            if (w_num >= 0)
                printf("PARENT: write %d\n", w_num);
            else {
                perror("pipe");
                break;
            }
        }

        close(pipefd[1]);
        printf("PARENT: close write\n");
        sleep(10);
    } else {
        perror("fork");
        return -1;
    }

    return 0;
}
