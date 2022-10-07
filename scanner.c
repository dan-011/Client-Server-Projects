#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define DEFAULT_START_PORT 1
#define DEFAULT_END_PORT   100
#define DEFAULT_HOST       "localhost"

#define handle_error(msg) \
           do { perror(msg); exit(-1); } while (0)

// Declare any additional macros and functions here
// TODO


int main(int argc, char* argv[])
{
    int start = DEFAULT_START_PORT;
    int end   = DEFAULT_END_PORT;
    char* servername = DEFAULT_HOST;

    if(argc < 4 && argc > 2){
        printf("Invalid Input\n");
        return 0;
    }
    if(argc == 4){
        servername = argv[1];
        start = atoi(argv[2]);
        end = atoi(argv[3]);
    }
    //printf("Host: %s\nStart: %d\nEnd: %d\n", servername, start, end);
    
    
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    

    for(int port = start; port <= end; port++){
        struct addrinfo *serverinfo;
        //char* port_str = (char*)calloc(128, sizeof(char));
        char port_str[128];
        sprintf(port_str, "%d", port);
        getaddrinfo(servername, port_str, &hints, &serverinfo);
        int sfd = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol);
        int status = connect(sfd, serverinfo->ai_addr, serverinfo->ai_addrlen);
        if(status < 0){
            close(sfd);
            freeaddrinfo(serverinfo);
            continue;
        }
        char msg[] = "GET /index.html HTTP/1.0\r\n\r\n";
        write(sfd, msg, strlen(msg));
        shutdown(sfd, SHUT_WR);
        //char* res = (char*)calloc(4, sizeof(char));
        char res[4];
        int bytes = read(sfd, res, 4);
        //shutdown(sfd, SHUT_RD);
        if(bytes < 4 || (res[0] != 'H' && res[1] != 'T' && res[2] != 'T' && res[3] != 'P')){
            //printf("%s\n", res);
            printf("found active non-HTTP port: %d\n", port);
        }
        else if(res[0] == 'H' && res[1] == 'T' && res[2] == 'T' && res[3] == 'P'){
            printf("found active HTTP port: %d\n", port);
        }
        close(sfd);
        freeaddrinfo(serverinfo);
        //free(res);
        //free(port_str);
    }

    exit(0);
}

