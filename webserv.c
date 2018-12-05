//Local Host: localhost:8080/file.cgi

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

				// stat file

				// send file to client ?
/*
				char send_buffer[1000];
				FILE *sendFile = fopen(request, "r");
				while( !feof(sendFile) )
				{
					int numread = fread(send_buffer, sizeof(unsigned char), 1000, sendFile);
					if( numread < 1 ) break; // EOF or error

					char *send_buffer_ptr = send_buffer;
					int numsent = send(acceptSocket, send_buffer_ptr, numread, 0);
					    
				}		
*/
				printf("Message sent\n");
 				close (acceptSocket);


				exit(EXIT_SUCCESS);
			}
			

		}

	}


	return;

}
