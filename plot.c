#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include "stringManip.h"
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>


extern char ** environ;

int main(int argv, char** argc)
{
	char * directory = argc[1];

	char* execDir = malloc(sizeof(char) * strlen(directory) + 1);
	strcpy(execDir, directory);
	strcat(execDir, "\0");
/*
	if (remove("file_data.dat") == 0)
		printf("data file delted\n");
	else
		printf("unable to delete file\n");
*/
	int pipefd[2];
	pid_t pid, wpid;
	int status;
	if (pipe(pipefd) == -1)
	{
		fprintf(stderr, "CGI: failed to make pipe | %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	remove("file_data.dat");
	remove("test_files.jpeg");

	if((pid = fork()) < 0)// error
	{
		fprintf(stderr, "CGI: error forking | %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	else if(pid == 0) // CHILD
	{
		close(pipefd[0]);
		dup2(pipefd[1],1);
		
		char *args[] = {"./hist\0", execDir, NULL };

		if(execve(&args[0][0], args, environ) < 0)
		{
			fprintf(stderr, "CGI: %s: %s\n", args[0], strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	else{ 
		do{
			wpid = waitpid(pid, &status, WUNTRACED);
		} while( !WIFEXITED(status) && !WIFSIGNALED(status));

		//sleep (2);

		printf("Content-Type: text/html\n\n");
		printf("<html><head><style>img {width: 40%%;}</style></head>\n");
		printf("<body style='background-color:#bbff99;'>\n");
		printf("<h1 style='font-size: 250%%;font-family:verdana;color:#009900;text-align:center;'>CS 410: Webserver</h1>\n");
		printf("<p style='font-size: 150%%;'></p>\n");
		printf("<center><img src='./test_files.jpeg' width='400' height='400'></center>\n</body>\n</html>\n");

	}
	  
	return 0;
}
