#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

//The Port given in the instructions that the server is listening too
#define PORT "8088" 
//Max number of bytes given in the intructions
#define MAXDATASIZE 100      


//Code Copied from the textbook
int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    //Changed buf (as seen in the textbook) to message, just made more sense
    char message[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int len;

    if (argc != 3) {
        fprintf(stderr, "usage: client hostname message\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    freeaddrinfo(servinfo); // All done with this structure
    
//-----------------------New Code-------------------------------

    //argv[2] is the message that the client inputs in the terminal 
    //that will be used to send to the server
    len = strlen(argv[2]);
    
    //Sends a message to the server using 'send' 
    if (send(sockfd, argv[2], len, 0) == -1) {
        perror("send");
        exit(1);
    }

    //Number of bytes received from the servers message
    numbytes = recv(sockfd, message, MAXDATASIZE - 1, 0);

    //Receive the modified message from the server using recv as seen in the textbook
    if (numbytes == -1) {
        perror("recv");
        exit(1);
    }

    // Null-terminate the received message
    message[numbytes] = '\0';

    //Prints the received message from the server
    printf("%s\n", message);
    
    //Prints the number of bits recieved
    printf("client: recieved %d bytes\n", numbytes);

    //Close the socket
    close(sockfd);

    return 0;
}

