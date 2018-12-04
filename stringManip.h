#ifndef STRINGMANIP_H
#define STRINGMANIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/*README stringToInt()
	+ converts a string integer to an int
*/
int stringToInt(char* line)
{
	int num = 0;
	int sign = 1;
	int i = 0;
	if (line[0] == '-'){
		sign = -1;
		i++;
	}
	while(line[i] != '\0'){
		num = (num*10) + (line[i] - '0');
		i++;
	}
	num = sign * num;

	return num;
}

/*README: intToString()
	+ converts an integer to a string for printing

*/
char* intToString(int input)
{
	char intLookup[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	int remainder;
	int sign = 1;
	int index = 0;

	char* revStr = malloc(sizeof(char) * 200);
	memset(revStr, 0, sizeof(char) * 200);

	if(input == 0){
		revStr[index] = '0';
		index++;
		revStr[index] = '\0';
		return revStr;
	}

	if(input < 0){
		sign = -1;
		input = input * -1;
	}

	while(input != 0){
		remainder = input % 10;
		revStr[index] = intLookup[remainder];
		index++;
		input = input / 10;
	}

	if(sign == -1){
		revStr[index] = '-';
		index++;
	}
	char* str = malloc(sizeof(char) * 200);
	memset(str, 0, sizeof(char) * 200);

	int i;
	for(i = 0; i < index; i++){
		str[i] = revStr[index - 1 -i];
	}
	str[i] = '\0';

	free(revStr);

	return str;

}


#endif