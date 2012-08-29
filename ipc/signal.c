#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MYSIG   SIGINT

void act_handler(int signum, siginfo_t *info, void *myact)
{
    printf("receive signal %d\n", signum);
    //sleep(5);
}

void sig_handler(int signum)
{
    printf("receive signal %d\n", signum);
}

void ignore_sig(int signum)
{
    sigset_t block;

    sigemptyset(&block);
    sigaddset(&block, signum);
    sigprocmask(SIG_BLOCK, &block, NULL);
}

void watch_sig(int signum)
{
    sigset_t block;

    sigemptyset(&block);
    sigaddset(&block, signum);
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

int main()
{
    struct sigaction act, old_act;
    int sig, flag, i;
    sigset_t pending;
    char buf[100];
    
    printf("pid is %d\n", getpid());

    sig = MYSIG;
    /* install signal action */
    sigemptyset(&act.sa_mask);
    /* ignore SIGTERM during sa_handler execution */
    //sigaddset(&act.sa_mask, SIGTERM);

    /* set signal hander with argument */
    //act.sa_flags = SA_SIGINFO;
    //act.sa_sigaction = act_handler;

    /* set signal hander without argument */
    act.sa_flags = 0;
    act.sa_flags |= SA_RESTART;
    act.sa_handler = sig_handler;

    //signal(sig, sig_handler);

    sigaction(sig, NULL, &old_act);
    if (old_act.sa_handler == SIG_IGN) {
        printf("old action is SIG_IGN\n");
        return 0;
    }
    if (sigaction(sig, &act, NULL) < 0) {
        perror("sigaction");
        return -1;
    }

    ignore_sig(MYSIG);
    printf("block signal %d\n", MYSIG);

    printf("send a signal ---> \n");
    kill(getpid(), MYSIG);
    printf("send a signal ---> \n");
    kill(getpid(), MYSIG);

    /* check unhandled signal */
    sigpending(&pending);
    for (sig = 1; sig < NSIG; sig++)
        if (sigismember(&pending, sig))
            psignal(sig, "Pending sig: ");
   
    watch_sig(MYSIG);
    printf("unblock signal %d\n", MYSIG);

    /* again: check unhandled signal */
    sigpending(&pending);
    for (sig = 1; sig < NSIG; sig++)
        if (sigismember(&pending, sig))
            psignal(sig, "Pending sig: ");
    if (sig == NSIG + 1)
        printf("no pending signal\n");

    memset(buf, 0, 100);
    i = read(0, buf, 100);
    if (i < 0)
        perror("read");
    else
        printf("read %d bytes: %s\n", i, buf);

    while (1) {
        sleep(5);
        printf("wait for the signal\n");
    }

    return 0;
}
