#include <stdio.h>
#include <sys/types.h>  // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h> // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h> // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h>	// for the waitpid() system call
#include <signal.h>	    // signal name macros

// Return codes
const int RC_SUCCESS = 1;
const int RC_ERROR = -1;

// Constants
const int MAX_NUM_CONNECTIONS = 5;
const int BUFFER_SIZE = 1024;

void error (char *msg) {
	perror(msg);
	exit(RC_SUCCESS);
}

int main (int argc, char* argv[]) {
	int sockfd, newsockfd, portno, pid;
	socklen_t clilen;

	struct sockaddr_in serv_addr, cli_addr;

	// Validate args
	if (argc < 2) {
         fprintf(stderr,"ERROR: no port provided\n");
         exit(RC_SUCCESS);
	}

	// Create socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == RC_ERROR)
		error("ERROR: could not open socket");

	// Clear address info
	memset((char *) &serv_addr, 0, sizeof(serv_addr));

	// Fill in address info
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	// Bind socket
	if ((bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)))
		== RC_ERROR)
		error("ERROR: could not bind");

	// Listen for connections
	listen(sockfd, MAX_NUM_CONNECTIONS);
	printf("Server listening on port: %d\n", portno);

	// Accept connections
	if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) == RC_ERROR)
		error("ERROR: could not accept connection");

	// Read client request
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);

	if ((read(newsockfd, buffer, BUFFER_SIZE - 1)) == RC_ERROR)
		error("ERROR: could not read from socket");

	printf("Here is the message: \n\n%s\n", buffer);

	// TODO: process HTTP request from buffer

	// Reply to client
	if ((write(newsockfd, "I got your message", 18)) == RC_ERROR)
		error("ERROR: could not write to socket");

	return RC_SUCCESS;
}