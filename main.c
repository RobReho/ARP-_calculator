#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

int operation(char op, int a, int b);
int result(char oper);
pid_t op_child;
int fdn, a, b;
char * myfifo = "/tmp/myfifo";

int main(int argc, char *argv[]){
   
    mkfifo(myfifo, 0666);
    char oper;
    printf("insert commands in the form \nOp1 a b\nOp2 c\nOp3 d\n...\nwhere:\np = sum\nm = subtraction\nt = product\nd = division\ninser 'x' to close\n\n");
    
    //get first command from stdin
    scanf("%c %i %i", &oper, &a, &b);
    operation(oper,a,b);

    //get following commands from stdin
    while(1){
        
        scanf("%c", &oper);
        if(oper == 'x'){        //quit option
            printf("closing...\n");
            close(fdn);     //close FIFO fd
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

    char a_[16], b_[16], fd[16];
    sprintf(a_,"%d",a); 
    sprintf(b_,"%d",b); 
    sprintf(fd,"%d",fdn);

    char *argv_p[]={"./sum",a_,b_,fd,(char*)NULL};
    char *argv_m[]={"./sub",a_,b_,fd,(char*)NULL};
    char *argv_t[]={"./pro",a_,b_,fd,(char*)NULL};
    char *argv_d[]={"./div",a_,b_,fd,(char*)NULL};

    switch(op){
    case 'p':
        op_child = fork();
        if (op_child < 0){    //error on function fork
            perror("Fork");
            return -1;
        }

        if (op_child == 0){ //child process: execute sum
            if(execvp(argv_p[0], argv_p) == -1){
                perror("main - Exec_Error: exec sum");
                exit(-1);
            }
        }else
            result('+');
        break;

    case 'm':
         op_child = fork();
        if (op_child < 0){    //error on function fork
            perror("Fork");
            return -1;
        }

        if (op_child == 0){ //child process: execute subtraction
            if(execvp(argv_m[0], argv_d) == -1){
                perror("main - Exec_Error: exec sub");
                exit(-1);
            }
        }else
            result('-');
        break;

    case 't':
        op_child = fork();
        if (op_child < 0){    //error on function fork
            perror("Fork");
            return -1;
        }

        if (op_child == 0){ //child process: execute product
            if(execvp(argv_t[0], argv_d) == -1){
                perror("main - Exec_Error: exec pro");
                exit(-1);
            }
        }else
            result('*');
        break;

    case 'd':
        op_child = fork();
        if (op_child < 0){    //error on function fork
            perror("Fork");
            return -1;
        }

        if (op_child == 0){ //child process: execute division
            if(execvp(argv_d[0], argv_d) == -1){
                perror("main - Exec_Error: exec div");
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
    int stat;
    int ans;

    //FIFO pipe
    fdn = open(myfifo,O_RDONLY);
    
    if(read(fdn, &ans, 80)==-1){    //read from FIFO
        perror("main - Failed to read from FIFO");
        close(fdn);
    }
    printf("%i %c %i = %i\n\n",a,oper,b,ans);
    a = ans;

    waitpid(op_child, &stat, 0);  //waiting for child 

    if (stat == 1)      //Verify if child process is terminated without error
    {
        printf("\nThe child process terminated with an error!.\n"); 
    }
}