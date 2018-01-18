//File: udpClient.cpp
//Author: Leon Becker
//E-mail: leonalexbecker@gmail.com

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

static const int BUFSIZE = 1024;

int main(int argc, char** argv){
    int sockfd, portno, n;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];

    //Check command line arguments
    if(argc != 3){
        fprintf(stderr, "usage: %s <hostname> <port> \n", argv[0]);
        exit(0);
    }

    hostname = argv[1];
    portno = atoi(argv[2]);

    //socket: create the socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        perror("Error opening socket");
        exit(1);
    }
    server = gethostbyname(hostname);
    if(server == NULL){
        fprintf(stderr, "ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    //build the server's internet address
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port =htons(portno);

    //get message from the user
    bzero(buf, BUFSIZE);
    printf("Please enter msg: ");
    fgets(buf, BUFSIZE, stdin);

    //send message to the server
    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&serveraddr, serverlen);
    if(n < 0){
        perror("Error in sendto");
        exit(1);
    }

    //print the server's reply
    n = recvfrom(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&serveraddr, &serverlen);
    if(n < 0){
        perror("Error in recvfrom");
        exit(1);
    }
    printf("Echo from server: %s", buf);
    return 0;

}
