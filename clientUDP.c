#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVERPORT "8088" // the port users will be connecting to
#define MAXDATASIZE 100 // max number of bytes the client can send

int main(int argc, char *argv[]) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    int len;
    char message[MAXDATASIZE];

    if (argc != 3) {
        fprintf(stderr,"usage: talker hostname message\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; //INET is IPv4
    hints.ai_socktype = SOCK_DGRAM; //Socket type is Datagram socket

    if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }

    if ((numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo);
    
    len = strlen(argv[2]);

    //The client sends the message the user inputted into the terminal to 
    //the server using sendto
    //The message is read in using argv[2] as it is at index 2 and the 3rd argument
    //inputted by the user
    //Ex: ./a.out  <ip-address> <the message>
    if((numbytes = sendto(sockfd, argv[2], len, 0, p->ai_addr,p->ai_addrlen)) == -1){
    	perror("sendto");
    	close(sockfd);
    	exit(1);
    }
    
    //If a client has successfully connect and found the sever it was looking for 
    //it prints the following message
    printf("client: connecting to %s\n", argv[1]);
    
    //The client then receives the captialzied message from the server using recvfrom
    if((numbytes = recvfrom(sockfd, message, MAXDATASIZE - 1, 0, NULL, NULL)) == -1){
    	perror("recvfrom");
    	exit(1);
    }
    
    message[numbytes] = '\0';
    
    //Prints the amount of bytes it recieved from the server
    printf("client: recieved %d bytes\n", numbytes);
    
    //Prints the received message
    printf("%s\n", message);
    
    //Close the socket
    close(sockfd);

    return 0;
}

