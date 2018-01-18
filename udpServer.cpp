//File: udpserver.cpp
//Author: Leon Becker
//E-mail: leonalexbecker@gmail.com

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static const int BUFSIZE = 1024;

int main(int argc, char**argv){
    int sockfd;                     //socket
    int portno;                     //port to listen on
    int clientlen;                  //byte size of the client's address
    struct sockaddr_in serveraddr;  //server's address
    struct sockaddr_in clientaddr;  //client address
    struct hostent *hostp;         //client host info
    char buf[BUFSIZE];              //message buffer
    char *hostaddrp;                //dotted decimal host address string
    int optval;                     //flag value for setsockopt
    int n;                          //message byte size

    //check command line arguments
    if(argc != 2){
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[1]);

    //Create the parent socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        perror("Error opening socket");
        exit(1);
    }

    //setsockopt: Rerun the server immediately after killing it
    //otherwise would have to wait 20 secs
    //eliminates "ERROR on binging: Address already in use" error.
    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));

    //Build the server's internet address
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);

    //bind: associate the parent socket with a port
    if(bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0 ){
        perror("Error on binding");
        exit(1);
    }

    //main loop: wait for a datagram, then echo it
    clientlen = sizeof(clientaddr);
    while(true){
        //recvfrom: receive a UDP datagram from a client
        bzero(buf,BUFSIZE);
        n = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr*) &clientaddr, &clientlen);

        if(n < 0){
            perror("Error in recvfrom");
            exit(1);
        }

        //gethostbyaddr: determine who sent the datagram
        hostp = gethostbyaddr((const char*)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);

        if(hostp == NULL){
            perror("Error on gethostbyaddr");
            exit(1);
        }
        
        hostaddrp = inet_ntoa(clientaddr.sin_addr);
        if(hostaddrp == NULL){
            perror("Error on inet_ntoa\n");
            exit(1);
        }

        printf("Server received datagram from %s (%s)\n", hostp->h_name, hostaddrp);
        printf("Server received %d/%d bytes: %s\n", strlen(buf), n, buf);

        //sentdo: echo the input back to the client
        n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&clientaddr, clientlen);
        if(n<0){
            perror("Error in sendto");
            exit(1);
        }
    }
}
