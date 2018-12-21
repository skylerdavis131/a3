#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h> 
#include <sys/types.h>
#include <sys/stat.h>

#include "stringManip.h"

/*
stty
ls /dev/tty.*

https://blog.nelhage.com/2009/12/a-brief-introduction-to-termios-termios3-and-stty/

https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c

http://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp
*/

/*Function Headers*/
int setUpArduino(struct termios* tty);


int main(int argv, char** argc){
	struct termios tty;
	int fd;
	int i;
	int outputLength;
	char* command;
	char* output;
	char buff[400];

	char* printHeader = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length:";

	memset(&tty, 0, sizeof(tty));
	memset(buff, 0, 400 * sizeof(char));

	fd = setUpArduino(&tty);

	sleep(5);
	if (argv == 2){
		write(fd, argc[1], strlen(argc[1]));
	}
	else if (argv >= 3){
		write(fd, argc[1], strlen(argc[1]));
		sleep(2);
		write(fd, argc[2], strlen(argc[2]));
	}
	sleep(2);

	write(fd,"status", 6);
	sleep(2);
	read(fd, buff, 400);
	sleep(4);

	// outputLength = strlen(printHeader) + strlen(buff) + strlen(intToString(strlen(buff)));
	// output = (char*)malloc(outputLength * sizeof(char));
	// strcat(strcat(printHeader, intToString(strlen(buff))), buff);

	write(1, buff, sizeof(buff));



	return 1;
}


/*README: setUpArduino()
	+ Function takes a termios structure as input, and alters the 
		structure members befor setting the attributes for 
		communication. 
	+ Function returns a file descriptor to the seriial
		link connected to the arduino
*/
int setUpArduino(struct termios* tty)
{
	int fd;
	char* portName;
	char* errorMessage;

	struct stat sb;

	/*Determin port*/
	portName = "/dev/cu.usbmodem14101";
	lstat(portName, &sb);
	

	/*Open the serial link*/
	if ( (fd = open(portName, O_RDWR | O_NOCTTY | O_SYNC) ) < 0 ){
		errorMessage = "-setUpArduino: couldn't open port\n";
		write(1, errorMessage, strlen(errorMessage));
		exit(EXIT_FAILURE);
	}



	if (tcgetattr(fd, tty) != 0){
		errorMessage = "-setUpArduino: error with tcgetattr\n";
		write(1, errorMessage, strlen(errorMessage));
		exit(EXIT_FAILURE);
	}

	/*Set the terminal IO atributes we want*/
	cfsetospeed(tty, 9600);
	cfsetispeed(tty, 9600);

	tty->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | 
					ISTRIP | INLCR | IGNCR | ICRNL | 
					IXON | IXOFF | IXANY | HUPCL);
	tty->c_oflag &= ~OPOST;
	tty->c_lflag &= ~(ICANON | ECHO | ECHONL | ISIG | IEXTEN);
	tty->c_cflag |= (CLOCAL | CREAD);
	tty->c_cflag &= ~(CSIZE | PARENB);
	tty->c_cflag |= CS8;
	tty->c_cc[VMIN] = 0;
	tty->c_cc[VTIME] = 0;




	if ( ( tcsetattr(fd, TCSANOW, tty) ) != 0 ){
		errorMessage = "-setUpArduino: error with tcsetattr\n";
		write(1, errorMessage, strlen(errorMessage));
		exit(EXIT_FAILURE);
	}


	/*
	Lock the file descriptor to other process
	because we dont want anything else messing
	with the information we send the arduino	
	*/
	if (flock(fd, LOCK_EX | LOCK_NB) == -1 ){
		errorMessage = "-setUpArduino: file descriptor for arduino's serial port is locked by another process";
		write(1, errorMessage, strlen(errorMessage));
		exit(EXIT_FAILURE);
	}


	return fd;

}













