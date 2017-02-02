#include <stdio.h>
#include <sys/types.h>  // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h> // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h> // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>	// for the waitpid() system call
#include <signal.h>	    // signal name 

// Return codes
const int RC_SUCCESS = 1;
const int RC_ERROR = -1;

// Constants
const int MAX_NUM_CONNECTIONS = 5;
const int BUFFER_SIZE = 1024;

// Content-Types
const char* HTML = "text/html";
const char* PLAIN_TXT = "text/plan";
const char* JPG = "image/jpg";
const char* GIF = "image/gif";

// Function headers
char* getFileRequested(char* buffer);
char* readFile(char* filename);
char* generateResponse(char* filename);
void error(char * msg);

// Main
int main(int argc, char* argv[]) {
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
	printf("Server listening on port: %d\n\n", portno);

	while (1) {
		// Accept connections
		if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) == RC_ERROR)
			error("ERROR: could not accept connection");

		// Read client request
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);

		if ((read(newsockfd, buffer, BUFFER_SIZE - 1)) == RC_ERROR)
			error("ERROR: could not read from socket");

		printf("Here is the message: \n\n%s\n", buffer);

		// Get the filename requested
		char* filename = getFileRequested(buffer);
		printf("filename: %s\n", filename);

		// Generate the response
		char* response = generateResponse(filename);

		// Send HTTP response to client
		if ((write(newsockfd, response, strlen(response))) == RC_ERROR)
			error("ERROR: could not write to socket");
	}

	return RC_SUCCESS;
}

// Helper Functions
void error(char *msg) {
	perror(msg);
	exit(RC_SUCCESS);
}

char* getFileRequested(char* buffer) {
	// Make a copy of buffer
	char buffercopy[BUFFER_SIZE];
	memset(buffercopy, 0, BUFFER_SIZE);

	strncpy(buffercopy, buffer, BUFFER_SIZE);

	// Get first token: HTML
	char* token = strtok(buffercopy, " ");
	
	// Get second token: /filename
	token = strtok(NULL, " "); 

	// Get rid of / in token
	token++;

	// If no filename, set to terminating character
	if (strlen(token) <= 0)
		token = "\0";

	return token;
}

char* readFile(char* filename) {
	char* buffer;
	FILE* fp;
	int filesize;

	// TODO: filename isn't working
	// Open file if it exists
	if ((fp = fopen("readme.txt", "r")) == NULL) {
		perror("fopen");
		return "File does not exist";
	}
	
	// Get filesize
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// Read file into buffer
	buffer = malloc(filesize + 1);
	fread(buffer, filesize, 1, fp);
	fclose(fp);

	return buffer;
}

char* generateResponse(char* filename) {
	char header[BUFFER_SIZE];
	char* file = readFile(filename);

	// Headers
	const char* HTTP = "HTTP/1.1";

	// Status codes
	const char* STATUS_OK = "200 OK";
	const char* STATUS_NOT_FOUND = "404 Not Found";

	return file;
}