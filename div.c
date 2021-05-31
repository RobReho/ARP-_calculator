#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

FILE *fp;
void logprint(char msg[]){
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    fprintf(fp, "%li : %li :: div: %s\n",current_time.tv_sec,current_time.tv_usec,msg); 
    fflush(stdout);
}

void sig_handler(int signo)
{
	if(signo == SIGUSR1){
		logprint("signal received, computing...");
	}
}

int main(int argc, char *argv[]){
    fp  = fopen ("data.log", "a+");
    if (fp == NULL) perror("log file couldn't open\n");
    logprint("invoked - div child running");

    signal(SIGUSR1, sig_handler);
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    int fdn = atoi(argv[3]);
    int ans;
    char * myfifo = "/tmp/myfifo";
    
    fdn = open(myfifo,O_WRONLY);
    if(fdn == -1){
        perror("div - failed to open FIFO\n");
        logprint("failed to open FIFO");
    }

    logprint("waiting for signal");
    pause();    //wait for signal

    ans = a / b;

    logprint("writing back");
    if(write(fdn, &ans, sizeof(ans))==-1){
        perror("div - failed to write on FIFO\n");
        logprint("failed to write on FIFO");
    }
    close(fdn);
    logprint("write end FIFO closed, child returning ");

return 0;
}