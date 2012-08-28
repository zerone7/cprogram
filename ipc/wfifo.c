#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#define FIFO_FILE   "/home/zero/fifotmp"
#define BUFSIZE     4097*2

int main()
{
    int fd;
    char wbuf[BUFSIZE];
    int w_num;

    memset(wbuf, 0, BUFSIZE);
    if (mkfifo(FIFO_FILE, 0777)) {
        perror("mkfifo");
        if (errno != EEXIST)
            return -1;
    }

    //fd = open(FIFO_FILE, O_WRONLY | O_NONBLOCK, 0);
    fd = open(FIFO_FILE, O_WRONLY, 0);
    while (fgets(wbuf, BUFSIZE, stdin)) {
        if (!strcmp(wbuf, "exit\n"))
            break;
        wbuf[strlen(wbuf)-1] = '\0';
        w_num = atoi(wbuf);
        w_num = write(fd, wbuf, w_num);
        if (w_num >= 0)
            printf("PARENT: write %d\n", w_num);
        else {
            perror("pipe");
            break;
        }
    }

    return 0;
}
