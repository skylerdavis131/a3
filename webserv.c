//Local Host: localhost:8080/file.cgi

#include <stdio.h>
#include <errno.h>
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
				// stat file
        if(strcmp(request,"/ls") == 0)
        {
          //opening current directory
          DIR * dir;
          if((dir = opendir(".")) != NULL)
          {
            struct dirent *dp;
            dp = readdir(dir);
            dp = readdir(dir);
            while((dp = readdir(dir)) != NULL)
            {
              char * prefix_for_dir = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length:";
              int length = strlen("./");
              int length2 = length + strlen(dp->d_name);
              char * address_for_dir = (char*)malloc(length2*sizeof(char));
              address_for_dir[0] = '.';
              address_for_dir[1] = '/';
              address_for_dir[2] = '\0';
              strcat(address_for_dir,dp->d_name);
              //printf("%s\n",address_for_dir);
              struct stat mybuf;
              if(lstat(address_for_dir,&mybuf)==0)
              {
                if(S_ISDIR(mybuf.st_mode)!=0)
                {
                  int length_of_dir_request = strlen(prefix_for_dir) + (length2 - length) + 5;// \n:3times wordcount: 2times

                  char * content_for_dir = (char*)malloc(length_of_dir_request*sizeof(char));
                  int iterate;
                  for(iterate = 0; iterate < strlen(prefix_for_dir); iterate++)
                  {
                    content_for_dir[iterate] = prefix_for_dir[iterate];
                  }
                  strcat(content_for_dir,intToString(length2 - length));
                  strcat(content_for_dir,"\n\n");
                  strcat(content_for_dir,dp->d_name);
                  strcat(content_for_dir,"\n");
                  write(1, content_for_dir, length_of_dir_request);
                  write(acceptSocket,content_for_dir,length_of_dir_request);
                  //free(content_for_dir);
                }
              }
              //free(address_for_dir);
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
