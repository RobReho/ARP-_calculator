#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

FILE *fp;
void logprint(char msg[],pid_t pid){
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    fprintf(fp, "%li : %li :: main (%i): %s\n",current_time.tv_sec,current_time.tv_usec,pid,msg); 
    fflush(stdout);
}
  
void sig_handler(int signo)
{
	if(signo == SIGUSR1){
		logprint("signal received, computing...",getpid());
	}
}

int main(int argc, char *argv[]){
    fp  = fopen ("data.log", "a+");
    if (fp == NULL) perror("log file couldn't open\n");
    logprint("invoked - sum child running",getpid());

    signal(SIGUSR1, sig_handler);
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    int fdw = atoi(argv[3]);
    int ans;
    char * myfifo = "/tmp/myfifo";

    logprint("waiting for signal",getpid());
    pause();    //wait for signal

    ans = a + b;
    
    logprint("writing back",getpid());
    if(write(fdw, &ans, sizeof(ans))==-1){
        perror("sum - failed to write on FIFO\n");
        logprint("failed to write to FIFO",getpid());
        close(fdw);
        exit(1);
    }
    
    logprint("write end FIFO closed, child returning ",getpid());

return 0;
}