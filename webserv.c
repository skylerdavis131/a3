//Local Host: localhost:8080/file.cgi

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>

#include "stringManip.h"

/*Function headers*/
void serverConnect(int port);

/*Global Variables*/


/*Function Definitions*/
int main(int argv, char** argc)
{
	int port;

	port = stringToInt(argc[1]);
	serverConnect(port);

	return 0;
}


void serverConnect(int port){

	int servFd, acceptSocket;
	pid_t pid;
	struct sockaddr_in address;
	char buffer[30000];
	ssize_t readBytes;
	int opt = 1;


	if ( (servFd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		fprintf(stderr, "-webserv: Error creating socket | %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (setsockopt(servFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(0);
	}

	memset((char*)&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);

	if (bind(servFd, (struct sockaddr *)&address, sizeof(address)) < 0){
		fprintf(stderr, "-webserv: Error binding socket | %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (listen(servFd, 5) < 0){
		fprintf(stderr, "-webserv: Error listening on socket | %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/*Server Loop*/
	while(1){

		if ( (acceptSocket = accept(servFd, (struct sockaddr*)&address, (socklen_t*)&address)) < 0 ){
			fprintf(stderr, "-webserv: Error accepting from socket | %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		else if (acceptSocket > 0){
			if ( (pid = fork()) < 0 ){ 
				fprintf(stderr, "-webserv: Error forking a child to handle request | %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			else if (pid == 0){ /*CHILD*/
				/*Handle the request here*/

				if ( (readBytes = read(acceptSocket, buffer, 30000)) == -1 ){
					fprintf(stderr, "-webserv: Error reading from socket | %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}

				
				/*Write to terminal the info we got from the client*/
				//write(1, buffer, strlen(buffer));

				/*Send message (Hello World!) to the client*/
				char* message = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length:22\n\nHello World!\nI am Jack";
				write(acceptSocket, message, strlen(message));

				// Receive request from client address
				char *request = strtok(buffer, " ");
				request = strtok(NULL, " ");
				request = request + 1; // removes slash from beginning of request
				printf("%s\n", request);

				// stat file
				struct stat statbuf;

				stat(request, &statbuf);

				if (S_ISDIR(statbuf.st_mode))
					// directory
					printf("is a directory\n");
					// list directory contents
				else if (S_ISREG(statbuf.st_mode))
					// regular file
					printf("is a regular file\n");
					// send file to client
				else
					// error in request
					// 404
					printf("could not find file\n");

				printf("Message sent\n");
 				close (acceptSocket);

				exit(EXIT_SUCCESS);
			}
			
		}

	}

	return;

}
