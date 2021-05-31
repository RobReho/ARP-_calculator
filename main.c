#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

FILE *fp;
int operation(char op, int a, int b);
int result(char oper);
pid_t op_child;
int a, b;
int fd[2];
char * myfifo = "/tmp/myfifo";

void logprint(char msg[]){
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    fprintf(fp, "%li : %li :: main: %s\n",current_time.tv_sec,current_time.tv_usec,msg); 
    fflush(stdout);
}
    
int main(int argc, char *argv[]){
    
    fp  = fopen ("data.log", "a+");
    if (fp == NULL) perror("log file couldn't open\n");

    //create unnamed pipe
    if(pipe(fd)==-1){
        perror("main: pipe opening failed\n");
        logprint("pipe opening failed");
        return -1;
    }

    char oper;
    printf("insert commands in the form \nOp1 a b\nOp2 c\nOp3 d\n...\nwhere:\np = sum\nm = subtraction\nt = product\nd = division\ninser 'x' to close\n\n");
    
    //get first command from stdin
    scanf("%c %i %i", &oper, &a, &b);
    operation(oper,a,b);

    //get following commands from stdin
    while(1){
        
        scanf("%c", &oper);
        if(oper == 'x'){        //quit option
            logprint("closing\n___________________________________");
            printf("closing...\n");
            close(fd[0]);     //close pipe fd
            close(fd[1]);
            exit(0);
        }
        else{
            scanf("%i", &b);
            operation(oper,a,b);
        }
    }

    return 0;
    
}

//this function recognises the command and forks the appropriate process
int operation(char op, int a, int b){

    char a_[16], b_[16], fdw[16];
    sprintf(a_,"%d",a); 
    sprintf(b_,"%d",b); 
    sprintf(fdw,"%d",fd[1]);

    char *argv_p[]={"./sum",a_,b_,fdw,(char*)NULL};
    char *argv_m[]={"./sub",a_,b_,fdw,(char*)NULL};
    char *argv_t[]={"./pro",a_,b_,fdw,(char*)NULL};
    char *argv_d[]={"./div",a_,b_,fdw,(char*)NULL};

    switch(op){
    case 'p':       //sum operation
        op_child = fork();

        if (op_child < 0){    //error on function fork
            perror("Fork");
            return -1;
        }

        if (op_child == 0){ //child process: execute sum
            if(execvp(argv_p[0], argv_p) == -1){
                perror("main - Exec_Error: exec sum");
                logprint("main - Exec_Error: exec sum");
                exit(-1);
            }
        }else{
            result('+');
        }
        break;

    case 'm':       //subtraction operation
         op_child = fork();
        if (op_child < 0){    //error on function fork
            perror("Fork");
            return -1;
        }

        if (op_child == 0){ //child process: execute subtraction
            if(execvp(argv_m[0], argv_d) == -1){
                perror("main - Exec_Error: exec sub");
                logprint("main - Exec_Error: exec sub");
                exit(-1);
            }
        }else
            result('-');
        break;

    case 't':       //product operation
        op_child = fork();
        if (op_child < 0){    //error on function fork
            perror("Fork");
            return -1;
        }

        if (op_child == 0){ //child process: execute product
            if(execvp(argv_t[0], argv_d) == -1){
                perror("main - Exec_Error: exec pro");
                logprint("main - Exec_Error: exec pro");
                exit(-1);
            }
        }else
            result('*');
        break;

    case 'd':       //division operation
        op_child = fork();
        if (op_child < 0){    //error on function fork
            perror("Fork");
            return -1;
        }

        if (op_child == 0){ //child process: execute division
            if(execvp(argv_d[0], argv_d) == -1){
                perror("main - Exec_Error: exec div");
                logprint("main - Exec_Error: exec div");
                exit(-1);
            }
        }else
            result('/');
        break;
    }

    return 0;
}

//this function gets the result computed by the child processes through a FIFO pipe
int result(char oper){
    struct timeval timestamp;     
    int stat;
    int ans;

    //close writing side of pipe
    //close(fd[1]);

    /*open FIFO pipe
    fd = open(myfifo,O_RDONLY);
    if(fdn == -1){
        perror("sum - failed to open FIFO\n");
        logprint("sum - failed to open FIFO");
    }*/

    usleep(100000);

    //signal child process 
    logprint("signaling child process");
    kill(op_child,SIGUSR1);

    if(read(fd[0], &ans, 80)==-1){    //read answer from FIFO
        perror("main - Failed to read from FIFO");
        logprint("Failed to read from FIFO");
        close(fd[0]);
        exit(0);
    }
    logprint("read from fifo");

    printf("%i %c %i = %i\n\n",a,oper,b,ans);
    a = ans;

    waitpid(op_child, &stat, 0);  //waiting for child to return
  

    if (stat == 1){      //Verify if child process is terminated without error
        printf("\nThe child process terminated with an error!.\n"); 
        logprint("The child process terminated with an error!.");
    }
}