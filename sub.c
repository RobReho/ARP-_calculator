#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
   
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    int fdn = atoi(argv[3]);
    int ans;
    char * myfifo = "/tmp/myfifo";
    
    ans = a - b;

    fdn = open(myfifo,O_WRONLY);
    if(fdn == -1){
        perror("sub - failed to open FIFO\n");
    }

    if(write(fdn, &ans, sizeof(ans))==-1){
        perror("sub - failed to write on FIFO\n");
    }
    close(fdn);

return 0;
}