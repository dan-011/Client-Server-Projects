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
#include <assert.h>

#define DEFAULT_PORT        "3100"
#define BACKLOG 10
#define BUF_LEN 80
#define MAX_REPLY_LEN 1000

#define handle_error(msg) \
           do { perror(msg); exit(-1); } while(0)

// Declare any additional macros and functions here
// TODO


double stddev(int count, double sum, double sumsq){
   if (count == 0) return 0.0;
   else{
      return sqrt((sumsq - ((sum*sum)/count))/(count - 1));
   }
}

double mean(double sum, int count){
   if (count == 0){
      return 0.0;
   }
   else return sum/count;
}

int main(int argc, char* argv[])
{
   char* port = DEFAULT_PORT;
   if(argc>1){
      port = argv[1];
   }
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

   while(1){ // loop
      int client = accept(lfd, NULL, NULL);
      if (client < 0){
         printf("accept went wrong\n");
         exit(0);
      }
      pid_t pid = fork();
      if (pid == 0){
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
         exit(0);
      }
      else{ // parent
         close(client);
         int status;
         pid_t deadChild;
         do {
            deadChild = waitpid(-1, &status, WNOHANG);
         } while(deadChild > 0);
      }
   }

   // Implement the server functionality here
   // TODO

   exit(0);
}
