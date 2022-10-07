#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <float.h>
#include <sys/select.h>
#include <assert.h>

#define DEFAULT_PORT        "3100"
#define DEFAULT_META_PORT   "3101"
#define BACKLOG 10
#define BUF_LEN 80
#define MAX_REPLY_LEN 1000

#define handle_error(msg) \
           do { perror(msg); exit(-1); } while (0)

// Declare any additional macros and functions here
// TODO

double stddev(int count, double sum, double sumsq){
   return sqrt((sumsq - ((sum*sum)/count))/(count - 1));
}

double mean(double sum, int count){
   return sum/count;
}

int max_fd(int sock1, int sock2, int pipe){
    //printf("sock1: %d, sock2: %d, pipe: %d\n", sock1, sock2, pipe);
    if(sock1 > sock2 && sock1 > pipe){
        //printf("sock1\n");
        return sock1 + 1;
    }
    else if(sock2 > sock1 && sock2 > pipe){
        //printf("sock2\n");
        return sock2 + 1;
    }
    else if(pipe > sock1 && pipe > sock2){
        //printf("pipe\n");
        return pipe + 1;
    }
    return -1;
}

int main(int argc, char* argv[])
{
   char* port      = DEFAULT_PORT;
   char* meta_port = DEFAULT_META_PORT;
   if(argc > 1)
       port = argv[1];
   if(argc > 2)
       meta_port = argv[2];

   // Implement the server functionality here
   // TODO

   struct addrinfo hints; // create hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *serverinfo;
    int check = getaddrinfo(NULL, port, &hints, &serverinfo); // set the address info
    if (check != 0){
        printf("getaddrinfo went wrong\n");
        exit(0);
    }
    int lfd = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol); // create the socket
    if (lfd < 0){
        printf("socket went wrong\n");
        exit(0);
    }
    int yes = 1;
    check = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); // set option to have port reuse
    if (check < 0){
        printf("setsockopt went wrong\n");
        exit(0);
    }
    check = bind(lfd, serverinfo->ai_addr, serverinfo->ai_addrlen); // bind the socket to the server address
    if (check < 0){
        printf("bind went wrong\n");
        exit(0);
    }
    freeaddrinfo(serverinfo);
    check = listen(lfd, BACKLOG); // prepare for listening
    if (check < 0){
        printf("listen went wront\n");
        exit(0);
    }
    
    int p[2];
    pipe(p);

    struct addrinfo hints2;
    memset(&hints2, 0, sizeof(hints2));
    hints2.ai_family = AF_INET;
    hints2.ai_socktype = SOCK_STREAM;
    hints2.ai_flags = AI_PASSIVE;

    struct addrinfo *s2;
    getaddrinfo(NULL, meta_port, &hints2, &s2);
    int sock = socket(s2->ai_family, s2->ai_socktype, s2->ai_protocol);
    int test2 = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &test2, sizeof(int));
    bind(sock, s2->ai_addr, s2->ai_addrlen);
    freeaddrinfo(s2);
    listen(sock, BACKLOG);

    int means_count = 0;
    double means_sum = 0.0;
    double means_sumsq = 0.0;

    while(1){
        //printf("%d %f %f\n", means_count, means_sum, means_sumsq);
        fd_set set;
        FD_ZERO(&set);
        FD_SET(sock, &set);
        FD_SET(p[0], &set);
        FD_SET(lfd, &set);
        
        select(max_fd(lfd, sock, p[0]), &set, NULL, NULL, NULL);
        if(FD_ISSET(lfd, &set)){
            //while(1){ // loop
                int client = accept(lfd, NULL, NULL);
                if (client < 0){
                    printf("accept went wrong\n");
                    exit(0);
                }
                pid_t pid = fork();
                if (pid == 0){
                    close(p[0]);
                    int count = 0;
                    double sum = 0.0;
                    double sumsq = 0.0;
                    write(client, "Stats server ready\n", 19);
                    while(1){
                        //write(client, "iter\n", 5);
                        char* buff = (char*)calloc(2048, sizeof(char));
                        int bytes = read(client, buff, 2048);
                        if(bytes > 80){
                        //printf("%s\n", buff);
                        write(client, "Error: long line\n", 17);
                        continue;
                        }
                        else{
                        //printf("%s", buff);
                        int index = 0;
                        char strnum[80];
                        char command[80];
                        int i = 0;
                        int valid = 1;
                        float num;
                        char isnum = 0;
                        char iscomm = 0;
                        if(buff[index] == ' ' || (buff[index] >= 48 && buff[index] <= 57)){ // if the first element is a space or a number
                            while(buff[index] != '\n'){
                                if(buff[index] == 46 || (buff[index] >= 48 && buff[index] <= 57)){
                                    strnum[i] = buff[index];
                                    i++;
                                }
                                else if(buff[index] != ' '){
                                    write(client, "Error: unrecognized command\n", 28);
                                    valid = 0;
                                    break;
                                }
                                index++;
                            }
                            if(valid){
                                buff[i] = '\0';
                                num = atof(strnum);
                                isnum = 1;
                            }
                        }
                        else{
                            while(buff[index] != '\n'){
                                command[index] = buff[index];
                                index++;
                            }
                            command[index] = '\0';
                            iscomm = 1;
                        }
                        if(isnum){
                            count++;
                            sum += num;
                            sumsq += (num*num);
                        }
                        else if(iscomm){ // count, sum, mean, stddev, exit
                            char c[80];
                            if(!strcmp(command, "count")){
                                sprintf(c, "%d\n", count);
                            }
                            else if(!strcmp(command, "sum")){
                                sprintf(c, "%.2lf\n", sum);
                            }
                            else if(!strcmp(command, "mean")){
                                sprintf(c, "%.2lf\n", mean(sum, count));
                            }
                            else if(!strcmp(command, "stddev")){
                                sprintf(c, "%.2lf\n", stddev(count, sum, sumsq));
                            }
                            else if(!strcmp(command, "exit")){
                                sprintf(c, "EXIT STATS: count %d mean %.2lf stddev %.2lf\n", count, mean(sum, count), stddev(count, sum, sumsq));
                                write(client, c, strlen(c));
                                char mean_str[128];
                                sprintf(mean_str, "%.2lf", mean(sum, count));
                                write(p[1], mean_str, strlen(mean_str)+1);
                                break;
                            }
                            else{
                                write(client, "Error: unrecognized command\n", 28);
                                continue;
                            }
                            write(client, c, strlen(c));
                        }
                        }
                        free(buff);
                    }
                    //shutdown(client, SHUT_RDWR); this may be needed

                    close(client);
                    close(p[1]);
                    exit(0);
                }
                else{ // in parent
                    close(client);
                    int status;
                    pid_t deadChild;
                    do {
                        deadChild = waitpid(-1, &status, WNOHANG);
                    } while(deadChild > 0);
                }
            //}
             
        }
       if(FD_ISSET(sock, &set)){
            int c2 = accept(sock, NULL, NULL);
            //write(c2, "connected\n", 10);
            char msg[128];
            sprintf(msg, "means_count %d mean_of_means %.2lf stddev_of_means %.2lf\n", means_count, mean(means_sum, means_count), stddev(means_count, means_sum, means_sumsq));
            write(c2, msg, strlen(msg));
            close(c2);
        }
        if(FD_ISSET(p[0], &set)){ // p[0]
            //printf("we get here\n");
            char* buff2 = (char*)calloc(2048, sizeof(char));
            int bytes_read = read(p[0], buff2, 2048);
            if(bytes_read == 0){
                free(buff2);
                continue;
            }
                    //printf("%s", buff2);
            if(!strcmp("-nan", buff2)){
                free(buff2);
                continue;
            }
            double lmean = atof(buff2);
            free(buff2);
            if(lmean > 0){
                means_count++;
                means_sum += lmean;
                means_sumsq += (lmean*lmean);
            //    printf("hi\n");
            }
        }
        //printf("loop");
    }
    //printf("we get here\n");
    close(p[0]);
    close(p[1]);
    exit(0);
}