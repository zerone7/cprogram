#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define FIFO_FILE   "/home/zero/fifotmp"
#define BUFSIZE     4097*2

int main()
{
    int fd;
    char rbuf[BUFSIZE];
    int num, rsize;

    fd = open(FIFO_FILE, O_RDONLY | O_NONBLOCK, 0);
    //fd = open(FIFO_FILE, O_RDONLY, 0);
    if (fd < 0) {
        perror("fifo open");
        return -1;
    }

    fgets(rbuf, BUFSIZE, stdin);
    rsize = atoi(rbuf);

    while (1) {
        memset(rbuf, 0, BUFSIZE);
        num = read(fd, rbuf, rsize);
        if (num < 0)
            perror("fifo read");
        printf("read %d bytes\n", num);
        sleep(1);
    }

    return 0;
}
