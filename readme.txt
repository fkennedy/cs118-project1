-------------------------------------
CS118 Project 1

Johnathan Estacio 404491851
Frederick Kennedy 404667930

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
1. Create a function to get the filename => getFileRequested
2. Create a function to read the file requested into a buffer => readFile
3. Create a function to generate the HTTP response => createResponse

-------------------------------------
NOTES

Sockets Tutorial: http://www.linuxhowtos.org/C_C++/socket.htm

