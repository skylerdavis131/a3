//Local Host: localhost:8080/info.html

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "stringManip.h"


void serverConnect(int port){

	int servFd, acceptSocket;
	pid_t pid;
	struct sockaddr_in address;
	char buffer[30000];
	ssize_t readBytes;


	if ( (servFd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		fprintf(stderr, "-webserv: Error creating socket | %s\n", strerror(errno));
		exit(EXIT_FAILURE);
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
				/*Handle the request here in the background*/
				setpgid(0,0);

				if ( (readBytes = read(acceptSocket, buffer, 30000)) == -1 ){
					fprintf(stderr, "-webserv: Error reading from socket | %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}

				/*Write to terminal the info we got from the client*/
				write(1, buffer, strlen(buffer));

				/*Send message (Hello World!) to the client*/
				char* message = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length:12\n\nHello World!";
				write(acceptSocket, message, strlen(message));
				printf("Message sent\n");
 				close (acceptSocket);


				exit(EXIT_SUCCESS);
			}
			

		}

	}


	return;





}

int main(int argv, char** argc)
{
	int port;

	port = stringToInt(argc[1]);
	serverConnect(port);

	return 0;
}