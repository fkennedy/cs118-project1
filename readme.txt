-------------------------------------
CS118 Project 1

Johnathan Estacio 404491851
Frederick Kennedy

-------------------------------------
TODO

Part B - Specs
1. Add a function to the web server 
	- that parses HTTP requests from the browser
	- creates an HTTP response message consisting of the requsested file preceded by header lines
	- sends the HTTP response to the client
	- make sure recognizes HTML files first
2. Add support to the function for GIF and JPEG images
	- browser should be able to display these images

Part B - Implementation
1. Create a HashTable with (key, value) = (String headerName, String value)
	- e.g. ("Accept", "text/html, application/...")

2. Fill HashTable by parsing HTTP request from client
	- char *strstr(const char *haystack, const char *needle) : finds occurence of needle in haystack

3. 

-------------------------------------
NOTES

Sockets Tutorial: http://www.linuxhowtos.org/C_C++/socket.htm

