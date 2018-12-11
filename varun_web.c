//Local Host: localhost:8080/file.cgi

#include <stdio.h>
#include <errno.h>
#include<fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include "stringManip.h"
#include <sys/stat.h>
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
				/*char* message = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length:23\n\nHello World!\nI am Jack\n";
				write(acceptSocket, message, strlen(message));
        */
				// Receive request from client address
				char *request = strtok(buffer, " ");
				request = strtok(NULL, " ");
        int i;
        char extensionbuf[6];
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

        write(1, request2, strlen(request2));
        write(1,"\n",strlen("\n"));

        if((strcmp(extensionbuf,".jpg") == 0) || (strcmp(extensionbuf,".jpeg") == 0))
        {
          printf("Here jpeg\n");
          int fd;
          if((fd = open(request2,O_RDONLY)) >= 0)
          {
						printf("Here jpeg\n");
						struct stat mybuf;
						if(fstat(fd,&mybuf) == 0)
						{
							char * content_length_length = intToString(mybuf.st_size);
							printf("Content size: %s\n",content_length_length);
							FILE * fp = fopen(request2,"r");
							char * content_for_jpg = (char*)malloc(stringToInt(content_length_length) * sizeof(char));
							int final_length = strlen("HTTP/1.1 200 OK\nContent-Type: image/jpeg\nContent-Length:") + strlen(content_length_length) + strlen("\nConnection: keep-alive\n\n");
							char * output_buffer = (char*)malloc(final_length*sizeof(char));
							sprintf(output_buffer,"HTTP/1.1 200 OK\nContent-Type: image/jpeg\nContent-Length:%s\nConnection: keep-alive\n\n",content_length_length);
							write(acceptSocket,output_buffer,final_length);
							fread(content_for_jpg,sizeof(char),mybuf.st_size + 1,fp);
							fclose(fp);
							write(acceptSocket,content_for_jpg,mybuf.st_size);
						}
          }
        }
        else
        {
          printf("Here Directory\n");
          struct stat mybuf;
  				if(stat(request2,&mybuf) == 0)
  				{
            printf("it works\n");
  					if(S_ISDIR(mybuf.st_mode)!=0)
  					{
              printf("It works\n");
  						DIR * dir;
  						if((dir = opendir(request2)) != NULL)
  						{
  							struct dirent * dp;
  							dp = readdir(dir);
  							dp = readdir(dir);
  							char * content_for_dir = (char*)malloc(sizeof(char));
  							content_for_dir[0] = '\n';
  							int length = 1;
                write(1,"Directory Entries:\n",strlen("Directory Entries:\n"));
  							while((dp = readdir(dir)) != NULL)
  							{
  								int length2 = strlen(dp->d_name);
  								int length3 = length2 + length + 1;
  								content_for_dir = (char*)realloc(content_for_dir,length3*sizeof(char));
  								strcat(content_for_dir,(dp->d_name));
  								content_for_dir[length3 - 1] = '\n';
  								length = length3;
                  //write(1,"\n",strlen("\n"));
  							}
                content_for_dir = (char*)realloc(content_for_dir,(length+1)*sizeof(char));
                content_for_dir[length] = '\0';
                write(1,content_for_dir,strlen(content_for_dir));
                printf("Size of content_for_dir: %d\n",strlen(content_for_dir));
  							char * content_length_length = intToString(strlen(content_for_dir));
                printf("Size of content_length_length: %d\n",strlen(content_length_length));
  							int final_length = length + strlen("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length:") + strlen(content_length_length) + 4;
  							char * output_buffer = (char*)malloc(final_length*sizeof(char));
  							sprintf(output_buffer,"HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length:%s\n\n%s",content_length_length,content_for_dir);
  							write(acceptSocket,output_buffer,final_length);
                //free(output_buffer);
                //free(content_length_length);
                //free(content_for_dir);
  						}
  					}
  				}
        }
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
				printf("\nMessage sent\n");
 				close (acceptSocket);
				exit(EXIT_SUCCESS);
			}
		}
	}
	return;
}
