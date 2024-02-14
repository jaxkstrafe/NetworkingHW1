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
#include <ctype.h>

#define MYPORT "8088" // the port users will be connecting to
#define MAXDATASIZE 100

// get sockaddr, IPv4
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char message[MAXDATASIZE];
    socklen_t addr_len;
    char s[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    //After this line the server is now waiting to recieve a message from a client
    printf("Server: waiting to recvfrom...\n");

    //When a client sends a message to the server the server
    //Receives the message from the client using recvfrom
    //And stores the sent message in 'message'
    addr_len = sizeof their_addr;
    if ((numbytes = recvfrom(sockfd, message, MAXDATASIZE - 1 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }

    //Prints the IP address it received the message from
    printf("Server: got packet from %s\n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
    
    
    //Prints the message it recieved from the client
    printf("Server: recived client message\"%s\"\n", message);
    
    //Coverts the message from the client to all uppercase as per the instructions
    //Using the c function toupper to change each letter to uppercase
    for(int i = 0; i < numbytes; i++){
    	message[i] = toupper(message[i]);
    }
    
    //The server sends the now all uppercase message back to the client using sendto
    if((numbytes = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
    	perror("sendto");
    	exit(1);
    }
    
    //Prints the number of bytes it sent to the client
    //Which is just the num of bytes contained in the message
    printf("Server: sent %d bytes back to the client\n", numbytes);

    //Close the socket
    close(sockfd);

    return 0;
}
