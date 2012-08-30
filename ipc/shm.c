#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>

#define MYFILE  "/home/zero/file"
#define SHM_SIZE    4096

int main(void)
{
    int shmid;
    key_t key;
    char *shmp;

    key = ftok(MYFILE, 0);
    if (key < 0)
        perror("ftok");

    key = 0x123456;
    if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return -1;
    }

    if ((shmp = shmat(shmid, NULL, 0)) == (void *)-1) {
        perror("shmat");
        return -1;
    }

    printf("shmp: %s\n", shmp);
    strcpy(shmp, "hello world!");

    if (shmdt(shmp))
        perror("shmdt");

    return 0;
}
