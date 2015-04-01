#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h> 
#include <stdlib.h>

#define BUFSIZE 512
#define PORT 7734
#define CONNECTIONS 5 /*Number of connection allowed*/

void doprocessing(int);

int main( int argc, char *argv[] )
{
   int sockfd, newsockfd, portno, clilen,pid;
   char buffer[BUFSIZE];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;

   portno = PORT;

   /*Information about the currently active peers*/
   struct peers
   {
      char hostname;
      int peerport;
      struct peers * next; 
   };

   /*Index of RFCs available at each peer, which may
   * contain multiple records of a given RFC, one record 
   * for each peer that contains the RFC
   */
   struct RFCs
   {
      int RFCno;
      char title;
      char hostname;
      struct RFCs * next;
   };
   
   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0)
      {
      perror("ERROR opening socket");
      exit(1);
      }
   else {printf("First socket() function call.\n");}
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));

   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   printf("Socket Initialization success!\n");
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
      {
      perror("ERROR on binding");
      exit(1);
      }
   else {printf("Host address binding success!\n");}
   
   /* Start listening for the clients, here
   * process will go in sleep mode and will wait
   * for the incoming connection
   */
   
   if(listen(sockfd,CONNECTIONS)<0)
   {
      perror("ERROR on listening");
      exit(1);
   }
   else{printf("listening...\n");}
   
   clilen = sizeof(cli_addr);
   
   while (1)
   {
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      if (newsockfd < 0)
         {
         perror("ERROR on accept");
         exit(1);
         }
      else{printf("Accepted a client!\n");}
      
      /* Create child process */
      pid = fork();
      if (pid < 0)
         {
         perror("ERROR on fork");
         exit(1);
         }
      
      if (pid == 0)
         {
         /* This is the client process */
         close(sockfd);
         doprocessing(newsockfd);
         exit(0);
         }
      else
         {
         close(newsockfd);
         }
   } /* end of while */
}

void doprocessing(int sock)
{
   int n;
   char buffer[BUFSIZE];
   
   bzero(buffer,BUFSIZE);
   
   n = read(sock,buffer,BUFSIZE-1);
   
   if (n < 0)
      {
      perror("ERROR reading from socket");
      exit(1);
      }
   
   printf("Here is the message: %s\n",buffer);
   n = write(sock,"I got your message",18);
   
   if (n < 0)
      {
      perror("ERROR writing to socket");
      exit(1);
      }
}

