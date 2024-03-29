# NetworkingHW1: Jack Dalton, Giovanni Smith
# serverTCP.c
### How to run
#### 1) First, compile the c code in the terminal 
#### gcc -o server serverTCP.c
#### 2) Then use the server object file to begin the server
#### ./server
#### 3) The server is now running and waiting for connections

# clientTCP.c
## How to Run
#### 1) First, compile the c code in the terminal
#### gcc -o client clientTCP.c
#### 2) Then to send a message to the server input the following into the command line
#### ./client (ip address of the server) (message)

# serverUDP.c
### How to run
#### 1) First, compile the c code in the terminal 
#### gcc -o server serverUDP.c
#### 2) Then use the server object file to begin the server
#### ./server
#### 3) The server is now running and waiting for connections

# clientUDP.c
## How to Run
#### 1) First, compile the c code in the terminal
#### gcc -o client clientUDP.c
#### 2) Then to send a message to the server input the following into the command line
#### ./client (ip address of the server) (message)



## Short Answer Questions
### (a) (5 points) What happens when you use the same port numbers on both sides: client and server? Do these ports interfere with each other?
#### The message won't send from the client

#### (b) (5 points) Set the port of the server side to less than 1024 and then try to run your code again. Why is it asking to enter your admin password to run the program?
#### The first 1024 ports are reserved, so Linux wants to limit this potentially risky action to only those who know what they're doing. It also may serve as an "an you sure?" type of confirmation protocol.
### (c) (5 points) TCP is a byte-stream oriented protocol and UDP is a message oriented protocol. Explain what does it mean with an example.
#### All the packets (bytes) have to come in order for TCP. However with UDP, the order of the packets doesn't matter, only the content of the message matters.




