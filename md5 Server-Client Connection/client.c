#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>


#define DEFAULT_PORT "3100"
#define DEFAULT_HOST "localhost"
#define MAX_LINE_LEN 1000
#define MD5SUM_LEN 32

#define handle_error(msg) \
           do { perror(msg); exit(-1); } while (0)


int sendFile(char* file_name, FILE* tx)
{
    FILE* f = fopen(file_name,"r");
    if(f == NULL)
       return -1;

    fseek(f,0,SEEK_END);
    int size = ftell(f);

    // send file size in network format
    fprintf(tx, "%d\n", size);

    // send file content
    rewind(f);
    for(int i=0; i<size; i++)
       fputc( fgetc(f), tx);

    fflush(tx);
    fclose(f);
    return 0;
}

int main(int argc,char **argv)
{
    char* servername = DEFAULT_HOST;
    char* port       = DEFAULT_PORT;

    if( argc > 1 ) servername = argv[1];
    if( argc > 2 ) port = argv[2];

    // prepare hints for getaddrinfo
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;


    // get server address info
    struct addrinfo *servinfo;
    int res = getaddrinfo(servername, port, &hints, &servinfo);
    if(res != 0)
        handle_error("client: getaddrinfo error");

    // create socket
    int sock_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if(sock_fd == -1)
        handle_error("client: socket error");

    // connect to the server (loop through servinfo list if more than one address)
    res = connect(sock_fd, servinfo->ai_addr, servinfo->ai_addrlen);
    if(res == -1)
        handle_error("client: connect error");

    // cleanup
    freeaddrinfo(servinfo); // done with this struct

    FILE* rx = fdopen(sock_fd, "r");
    FILE* tx = fdopen(dup(sock_fd), "w");

    // exchange messages
    while(1)
    {
        char line[MAX_LINE_LEN];
        char file_name[100];

        // fgets reads in the newline and adds NUL terminator
        fgets(line, sizeof(line), stdin);
        sscanf(line, "%100s", file_name);
        if( feof(stdin) )
            break;

        // send the file over the socket
        int ret = sendFile(file_name, tx);
        if( ret == -1 || feof(tx) )
            break;

        // get md5sum from server
        for(int i=0; i<MD5SUM_LEN; i++)
            putchar(getc(rx));

        putchar('\n');
        fflush(stdout);

        if( feof(rx) )
            break;
    }

    fclose(rx);
    fclose(tx);

    exit(0);
}

