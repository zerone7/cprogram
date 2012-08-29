#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MSGPATH     "/home/zero/examples.desktop"

struct msgbuf{
    int mtype;
    char mtext[4];
};

void msg_stat(int msgid)
{
    int ret;
    struct msqid_ds msg_info;

    sleep(1);
    ret = msgctl(msgid, IPC_STAT, &msg_info);
    if (ret == -1) {
        perror("msgctl IPC_STAT");
        return;
    }

    printf("msg key is 0x%x\n", msg_info.msg_perm.__key);
    printf("current number of bytes in queue is %ld\n", msg_info.__msg_cbytes);
    printf("number of messages in queue is %d\n", (int)msg_info.msg_qnum);
    printf("max num of bytes allowed in queue is %d\n", (int)msg_info.msg_qbytes);
    printf("pid of last msgsnd is %d\n", msg_info.msg_lspid);
    printf("pid of last msgrcv is %d\n", msg_info.msg_lrpid);
    printf("last msgsnd time is %s", ctime(&msg_info.msg_stime));
    printf("last msgrcv time is %s", ctime(&msg_info.msg_rtime));
    printf("last change time is %s", ctime(&msg_info.msg_ctime));
    printf("msg uid is %d\n", msg_info.msg_perm.uid);
    printf("msg gid is %d\n", msg_info.msg_perm.gid);
    printf("\n");
}

int main()
{
    int gflags, sflags, rflags, msgid;
    key_t key;
    struct msgbuf msg_sbuf, msg_rbuf;
    struct msqid_ds msg_ginfo, msg_sinfo;

    memset(&msg_sbuf, 0, sizeof(msg_sbuf));
    memset(&msg_rbuf, 0, sizeof(msg_sbuf));

    key = ftok(MSGPATH, 'k');
    gflags = IPC_CREAT;
    msgid = msgget(key, gflags | 0666);
    if (msgid == -1) {
        perror("msgget");
        return -1;
    }
    msg_stat(msgid);

    /* send message to message queue */
    sflags = IPC_NOWAIT;
    msg_sbuf.mtype = 10;
    msg_sbuf.mtext[0] = 'a';
    if (msgsnd(msgid, &msg_sbuf, sizeof(msg_sbuf.mtext), sflags) < 0)
        perror("msgsnd");
    msg_stat(msgid);

    /* read message from message queue */
    rflags = IPC_NOWAIT | MSG_NOERROR;
    if (msgrcv(msgid, &msg_rbuf, sizeof(msg_sbuf.mtext), 10, rflags) < 0) {
        perror("msgrcv");
        return -1;
    }
    msg_stat(msgid);

    /* root can set default value of msg_qbytes */
    msg_sinfo.msg_perm.uid = 8;
    msg_sinfo.msg_perm.gid = 8;
    msg_sinfo.msg_qbytes = 16388;
    if (msgctl(msgid, IPC_SET, &msg_sinfo) < 0) {
        perror("msgctl IPC_SET");
        return -1;
    }
    msg_stat(msgid);
    
    /* remove message queue */
    if (msgctl(msgid, IPC_RMID, NULL) < 0)
        perror("msgctl IPC_RMID");

    return 0;
}
