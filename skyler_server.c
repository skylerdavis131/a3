//Local Host: localhost:8080/file.ext

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include "stringManip.h"
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
/*Function headers*/
void serverConnect(int port);

/*Global Variables*/
extern char ** environ;

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
				write(1, buffer, strlen(buffer));

				/*Send message (Hello World!) to the client*/
				//char* message = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length:23\n\nHello World!\nI am Jack\n";
				//write(acceptSocket, message, strlen(message));
        
				// Receive request from client address
				char *http_request = strtok(buffer, " ");

				if (strcmp(http_request, "GET") != 0)
				{
					char * html_501 = "<div id='main'><div class='fof'><h1>Error 501</h1></div></div>\n";
					char * output_length = intToString(strlen(html_501));
					char * output_buffer = (char*)malloc(strlen(html_501) * sizeof(char));
					sprintf(output_buffer, "HTTP/1.1 501 Not Implemented\nContent-Type: text/html\nContent-Length: %s\n\n%s\n", output_length, html_501);
					write(acceptSocket, output_buffer, strlen(output_buffer));
				}

				char *request = strtok(NULL, " ");

				request = request + 1;
				char extensionbuf[20]; // holds extension of request

				printf("\nrequest: %s\n", request);

				// stat file

				struct stat statbuf;

				stat(request, &statbuf);

				if (S_ISDIR(statbuf.st_mode))
					// directory
					printf("dir\n");
				else if (S_ISREG(statbuf.st_mode))
					// regular file
					printf("reg\n");
				else // 404 ERROR
				{
					char * html_404 = "<div id='main'><div class='fof'><h1>Error 404</h1></div></div>\n";
					char * output_length = intToString(strlen(html_404));
					char * output_buffer = (char*)malloc(strlen(html_404) * sizeof(char));
					sprintf(output_buffer, "HTTP/1.1 404 Not Found\nConnection: close\nContent-Type: text/html\nContent-Length: %s\n\n%s\n", output_length, html_404);
					write(acceptSocket, output_buffer, strlen(output_buffer));
				}
        		printf("past 404\n");

		        int i;
		        for(i = 0; i < strlen(request); i++)
		        {
		          if(request[i] == '.')
		            strcpy(extensionbuf,&request[i]);
		        }
		        char * request2 = (char *)malloc((strlen(request) + 2) * sizeof(char));
		        request2[0] = '.';
		        for(i = 1; i <= strlen(request); i++)
		        {
		          request2[i] = request[i-1];
		        }
		        request2[strlen(request) + 1] = '\0';
		        printf("\nextenstion: %s\n", extensionbuf);

		        

		        if(strcmp(extensionbuf,".html") == 0) // checks if request is html
		        {
		          printf("is html file\n");
		          int fd;
		          if((fd = open(request, O_RDONLY)) >= 0)
		          {
								printf("opened html file\n");
								struct stat mybuf;
								if(fstat(fd, &mybuf) == 0)
								{
									char * content_length = intToString(mybuf.st_size);
									printf("Content size: %s\n", content_length);

									FILE * fp = fopen(request,"r");
									char * content_for_html = (char*)malloc(stringToInt(content_length) * sizeof(char));
									int final_length = strlen("HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length:") + strlen(content_length) + strlen("\nConnection: keep-alive\n\n");
									char * output_buffer = (char*)malloc(final_length * sizeof(char));
									sprintf(output_buffer, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length:%s\nConnection: keep-alive\n\n", content_length);
									write(acceptSocket, output_buffer, final_length);
									fread(content_for_html, sizeof(char), mybuf.st_size + 1, fp);
									fclose(fp);
									write(acceptSocket, content_for_html, mybuf.st_size);
								}
		          }
		        }
				else if((strcmp(extensionbuf,".jpg") == 0) || (strcmp(extensionbuf,".jpeg") == 0) || (strcmp(extensionbuf,".gif") == 0))
		        {
		          int fd;
		          if((fd = open(request,O_RDONLY)) >= 0)
		          {
								printf("Here jpeg or gif\n");
								struct stat mybuf;
								if(fstat(fd,&mybuf) == 0)
								{
									char * content_length_length = intToString(mybuf.st_size);
									printf("Content size: %s\n",content_length_length);
									FILE * fp = fopen(request,"r");
									char * content_for_jpg = (char*)malloc(stringToInt(content_length_length) * sizeof(char));
									int final_length = strlen("HTTP/1.1 200 OK\nContent-Type: image/jpeg\nContent-Length:") + strlen(content_length_length) + strlen("\nConnection: keep-alive\n\n");
									if(strcmp(extensionbuf,".gif") == 0)
									{
										final_length--;
									}
									char * output_buffer = (char*)malloc(final_length*sizeof(char));
									if(strcmp(extensionbuf,".gif") == 0)
									{
										sprintf(output_buffer,"HTTP/1.1 200 OK\nContent-Type: image/gif\nContent-Length:%s\nConnection: keep-alive\n\n",content_length_length);
									}
									else
									{
										sprintf(output_buffer,"HTTP/1.1 200 OK\nContent-Type: image/jpeg\nContent-Length:%s\nConnection: keep-alive\n\n",content_length_length);
									}
									write(acceptSocket,output_buffer,final_length);
									fread(content_for_jpg,sizeof(char),mybuf.st_size + 1,fp);
									fclose(fp);
									write(acceptSocket,content_for_jpg,mybuf.st_size);
								}
		          }
		        }
		        else if(extensionbuf[1] == 'c' && extensionbuf[2] == 'g' && extensionbuf[3] == 'i') // checks if request is cgi
		        {
		          if (extensionbuf[4] == '?')
		          {

		          }
		          struct stat mybuf;
					if(stat( ,&mybuf) == 0)
					{
						char ** args;
						int pipefd[2];
						pid_t pid, wpid;
						int status;
						if (pipe(pipefd) == -1)
						{
							fprintf(stderr, "CGI: failed to make pipe | %s\n", strerror(errno));
							exit(EXIT_FAILURE);
						}
						if((pid = fork()) < 0)// error
						{
							fprintf(stderr, "CGI: error forking | %s\n", strerror(errno));
							exit(EXIT_FAILURE);
						}
						else if(pid == 0) // CHILD
						{
							close(pipefd[0]);
							dup2(pipefd[1],1);
							args = (char**)malloc(2 * sizeof(char*));
							args[0] = (char*)malloc((strlen(request) + 1) * sizeof(char));
							for(i = 0; i < strlen(request); i++)
							{
								args[0][i] = request[i];
							}
							args[0][i] = '\0';
							args[1] = '\0';

							//args[1][0] = '\0';
							if(execve(&args[0][0],args,environ) < 0)
							{
								fprintf(stderr, "CGI: %s: %s\n", args[0], strerror(errno));
								free(args);
								exit(EXIT_FAILURE);
							}
						}
						else // Parent
						{
							close(pipefd[1]);
							do
							{
								wpid = waitpid(pid, &status, WUNTRACED);
							}
							while( !WIFEXITED(status) && !WIFSIGNALED(status));
						}
						char bufvar[80000];
						if((readBytes = read(pipefd[0],bufvar,80000)) == -1)
						{
							fprintf(stderr, "CGI: Error reading from socket | %s\n", strerror(errno));
							exit(EXIT_FAILURE);
						}
					
						char * output_length = intToString(strlen(bufvar));
						char * output_buffer = (char*)malloc(strlen(bufvar) * sizeof(char));
						sprintf(output_buffer, "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length:%s\n\n%s\n", output_length, bufvar);
						write(acceptSocket, output_buffer, strlen(output_buffer));
						write(1, bufvar, strlen(bufvar));
						

					}
					else
					{
						fprintf(stderr, "CGI: File doesn't exists %s\n", strerror(errno));
						exit(EXIT_FAILURE);
					}
		        }

				printf("\nMessage sent\n");
 				close (acceptSocket);
				exit(EXIT_SUCCESS);
			}


		}

	}

	return;

}
