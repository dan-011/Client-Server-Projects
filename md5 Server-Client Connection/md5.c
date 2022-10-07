#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/wait.h>

int main(int argc, char** argv){
    int p[2];
    pipe(p);
    int p2[2];
    pipe(p2);
    pid_t pid = fork();

    if(pid == 0){
        dup2(p[0], 0);
        close(p[1]);
        close(p[0]);
        dup2(p2[1], 1);
        close(p2[0]);
        close(p2[1]);

        execl("/bin/sh", "sh", "-c", "md5sum\n", (char *) 0);

    }
    else{
        char str[128];
        scanf("%s", str);
        close(p[0]);
        close(p2[1]);
        write(p[1], str, strlen(str));
        close(p[1]);
        char str2[128];
        int bytes = read(p2[0], str2, 128);
        str2[bytes] = '\0';
        printf("%s", str2);
        //close(p[1]);
        //close(p2[0]);
        return 0;
    }
}