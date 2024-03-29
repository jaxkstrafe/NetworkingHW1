#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
//Port users will be connecting to
#define PORT "8088" 
//How many pending connections queue will hold
//Although not necessary just including it since it was in the textbook
#define BACKLOG 10    
//Max number of bytes we can receive at once
#define BUFFERSIZE 100  

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    int sockfd, new_fd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    char buf[BUFFERSIZE];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM; //Socket type is stream socket
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept error");
            continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) {
        close(sockfd);
            
// ------------------------------------------------------------------------
	//Number of bytes received from the clients message
	int numBytes = recv(new_fd, buf, BUFFERSIZE, 0);

        //Receive message from the client using 'recv'
        if (numBytes == -1) {
            perror("recv error");
            exit(1);
        }

	//Uses the c function toupper which was imported using #include <ctype.h> to
	//capitalize every letter in the message
        for(int i = 0; i < numBytes; i++){
    	    buf[i] = toupper(buf[i]);
        }
	    
	    buf[numBytes] = '\0'; // Null-terminate the modified message

	    printf("server: message received '%s' \n", buf);


            //Send the received message back to the client using send()
            if (send(new_fd, buf, strlen(buf), 0) == -1) {
                perror("send error");
                exit(1);
            }

            exit(0);
        }
	//Close the new socket
        close(new_fd);
    }

    return 0;
}
