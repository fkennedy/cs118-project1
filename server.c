#include <stdio.h>
#include <sys/types.h>  // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h> // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h> // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>	// for the waitpid() system call
#include <signal.h>	    // signal name
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

// Return codes
#define RC_SUCCESS 1
#define RC_ERROR -1

// Return strings
#define FILE_NOT_FOUND "File not found."
#define STATUS_NOT_FOUND_RESPONSE "File does not exist."
#define FILE_TYPE_NOT_SUPPORTED "File type not supported."

// Constants
#define MAX_NUM_CONNECTIONS 5
#define BUFFER_SIZE 1024
#define CARRIAGE_RETURN "\r\n"
#define SPACE " "

// Content-Types
#define HTML "text/html"
#define PLAIN_TXT "text/plain"
#define JPG "image/jpg"
#define GIF "image/gif"

// Status codes
#define STATUS_OK "200 OK"
#define STATUS_NOT_FOUND "404 Not Found"

// Function headers
char* getFileRequested(char* buffer);
char* readFile(const char* filename);
char* generateResponse(int sockfd, const char* filename);
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
		char* response = generateResponse(newsockfd, filename);

		// Send HTTP response to client
		// if ((write(newsockfd, response, strlen(response))) == RC_ERROR)
		// 	error("ERROR: could not write to socket");
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

char* readFile(const char* filename) {
	char* buffer;
	FILE* fp;
	int filesize;

	// TODO: filename isn't working
	// Open file if it exists
	if ((fp = fopen(filename, "r")) == NULL)
		return FILE_NOT_FOUND;
	
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

time_t getLastModified(const char *filename) {
	struct stat statbuf;
	if (stat(filename, &statbuf) == RC_ERROR)
		perror(filename);
	return statbuf.st_mtime;
}

char* generateResponse(int sockfd, const char* filename) {
	char response[BUFFER_SIZE];
	memset(response, 0, BUFFER_SIZE);

	// Headers
	char header[BUFFER_SIZE];
	char* http;
	char* status;
	char* date;
	time_t current_time;
	char* lm;
	time_t last_modified;
	char* server;
	int contentlen;
	char* contenttype;
	char* connection;

	http = "HTTP/1.1";

	time(&current_time);
	date = ctime(&current_time);

	connection = "Closed";

	// TODO: server

	char* file = readFile(filename);

	// File not found
	if (strcmp(file, FILE_NOT_FOUND) == 0) {
		status = STATUS_NOT_FOUND;
		contentlen = strlen(STATUS_NOT_FOUND_RESPONSE);
		contenttype = PLAIN_TXT;
	}

	// File found
	else {
		status = STATUS_OK;
		contentlen = strlen(file);
		
		last_modified = getLastModified(filename);
		lm = ctime(&last_modified);
		
		char* filext = strrchr(filename, '.') + 1;
	
		if (strcmp(filext, "html") == 0) 
			contenttype = HTML;
		else if (strcmp(filext, "txt") == 0) 
			contenttype = PLAIN_TXT;
		else if (strcmp(filext, "jpg") == 0) 
			contenttype = JPG;
		else if (strcmp(filext, "gif") == 0) 
			contenttype = GIF;
		else {
			status = STATUS_NOT_FOUND;
			contentlen = strlen(STATUS_NOT_FOUND_RESPONSE);
			contenttype = PLAIN_TXT;
		}
	}

	// Create HTTP Response
	char* traverse = response;

	memcpy(traverse, http, strlen(http)); traverse += strlen(http);
	memcpy(traverse, SPACE, strlen(SPACE)); traverse += strlen(SPACE);
	// memcpy(traverse, status, strlen(status)); traverse += strlen(status);
	// memcpy(traverse, CARRIAGE_RETURN, strlen(CARRIAGE_RETURN)); traverse += strlen(CARRIAGE_RETURN);

	printf("\n\n%s\n\n", response);

	// TODO: Send HTTP Response
	// TODO: Send file

	return "sup";
}