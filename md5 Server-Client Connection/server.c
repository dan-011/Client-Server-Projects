#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/wait.h>


#define DEFAULT_PORT   "3100"
#define BACKLOG 10

#define handle_error(msg) \
         do { perror(msg); exit(-1); } while (0)

// Declare any additional macros and functions here
// TODO


int main(int argc, char* argv[])
{
   char* port = DEFAULT_PORT;
   if(argc>1)
      port = argv[1];

   // Implement the server functionality here
   // TODO

   struct addrinfo hints;
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;

   struct addrinfo *serverinfo;
   getaddrinfo(NULL, port, &hints, &serverinfo);
   int lfd = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol);
   int yes = 1;
   setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
   bind(lfd, serverinfo->ai_addr, serverinfo->ai_addrlen);
   freeaddrinfo(serverinfo);

   listen(lfd, BACKLOG);
   while(1){
      int client = accept(lfd, NULL, NULL);
      pid_t pid = fork();
      if(pid == 0){
            close(lfd);
            FILE* rx = fdopen(client, "r");
            FILE* tx = fdopen(dup(client), "w");
      while(1){
               int p_c[2];
            int c_p[2];
               pipe(p_c);
            pipe(c_p);

         pid_t md5_pid = fork();
         if(md5_pid == 0){ // in md5 process
            dup2(p_c[0], 0);
            close(p_c[0]);
            close(p_c[1]);
            dup2(c_p[1], 1);
            close(c_p[0]);
            close(c_p[1]);
            close(client);
            close(lfd);
            /*char str[10000];
            int bytes = read(0, str, 10000);
            str[bytes] = '\0';
            write(1, str, strlen(str));
            return 0; */
            //system("md5sum\n");
            //printf("Something went wrong\n");
            //return 0
            //exit(0);
            execl("/bin/sh", "sh", "-c", "md5sum", (char *) 0);
         }
         else{ // back in child process
                        //char rec[1000];
               int index = 0;
               char str_bytes[128];
               int c;
               char after_newline = 0;
               char* content;
               int num_bytes;
               while((c = fgetc(rx)) != EOF){
                  if((char)c == '\n' && !after_newline){
                     str_bytes[index] = '\0';
                     num_bytes = atoi(str_bytes);
                     content = (char*)calloc(num_bytes, sizeof(char));
                     after_newline = 1;
                     index = 0;
                  }
                  else if(after_newline){
                     content[index] = (char)c;
                     index++;
                  }
                  else if(!after_newline){
                     str_bytes[index] = (char)c;
                     index++;
                  }
               }
               /*while((c = fgetc(rx)) != (int)'\n'){
                  str_bytes[index] = (char)c;
                  index++;
               }
               str_bytes[index] = '\0';
               char content[num_bytes];
               index = 0;
               while((c = fgetc(rx)) != EOF){
                  content[index] = (char)c;
                  index++;
               }*/
               //printf("BYTES: %d\n%s", num_bytes, content);
               //fflush(stdout);
               //printf("%s", content);
               write(p_c[1], content, num_bytes);
               close(p_c[1]);
               free(content);
               char encoding[128];
               while(1){
                  int bytes_read = read(c_p[0], encoding, 128);
                  encoding[bytes_read] = '\0';
                  if(encoding[bytes_read-1] == '\n'){
                     break;
                  }
               }
               //int bytes = read(c_p[0], encoding, 128);
               //encoding[bytes] = '\0';
               int i = 0;
               while(encoding[i] != ' '){
                  i++;
               }
               encoding[i] = '\0';
               printf("%s\n", encoding);
               fprintf(tx, "%s", encoding);
               fflush(tx);
               //shutdown(client, SHUT_WR);
               //int s;
               //wait(&s);
            }
            close(client);
            fclose(rx);
            fclose(tx);
            exit(0);
         }
      }
      else{
         fprintf(stderr, "Forked %d\n", pid);
         close(client);
         int status;
         pid_t deadchild;
         do {
            deadchild = waitpid(-1, &status, WNOHANG);
            if(deadchild > 0){
               fprintf(stderr, "Reaped child with PID %d\n", deadchild);
            }
         } while(deadchild > 0);
      }
   }


   exit(0);
}
