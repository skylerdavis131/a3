#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include "stringManip.h"

#define NUM_POINTS 7
#define NUM_COMMANDS 12

int reg = 0;
int direct = 0;
int links = 0;
int fifo = 0;
int sock = 0;
int block = 0;
int character = 0;

void listdir(const char *name)
{
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
    	lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {

        	direct++;
            
            char path[1024];

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);

            //printf("%*s[%s]\n", indent, "", entry->d_name);
            listdir(path);
        } 
        else if (S_ISREG(statbuf.st_mode))
            reg++;
        else if (S_ISLNK(statbuf.st_mode))
        	links++;
        else if (S_ISFIFO(statbuf.st_mode))
        	fifo++;
        else if (S_ISSOCK(statbuf.st_mode))
        	sock++;
        else if (S_ISBLK(statbuf.st_mode))
        	block++;
        else if (S_ISCHR(statbuf.st_mode))
        	character++;
    }

    closedir(dir);


}


int main(int argv, char** argc)
{
	char * directory = argc[1];

	listdir(directory);

	printf("%d\n", reg);
    printf("%d\n", direct);
    printf("%d\n", links);
    printf("%d\n", fifo);
    printf("%d\n", sock);
    printf("%d\n", block);
    printf("%d\n", character);

    FILE * temp = fopen("file_data.dat", "w");
    int color = 0x009900;
    // write the data to a data file
    fprintf(temp, "regular\t%d\t%d\n", reg, color);
    fprintf(temp, "directory\t%d\t%d\n", direct, color);
    fprintf(temp, "link\t%d\t%d\n", links, color);
    fprintf(temp, "fifo\t%d\t%d\n", fifo, color);
    fprintf(temp, "socket\t%d\t%d\n", sock, color);
    fprintf(temp, "block\t%d\t%d\n", block, color);
    fprintf(temp, "character\t%d\t%d\n", character, color); 

	char * commandsForGnuplot[] = {
		"set term jpeg",
		"set output 'test_files.jpeg'",
		"set title 'File Count'",
		"unset key",
		"set boxwidth 0.8",
		"set style fill solid",
		"set xrange [-1:7]",
		"set xtics nomirror",
		"set grid y",
		"set ylabel 'Count'",
		"plot 'file_data.dat' using 0:2:3:xtic(1) with boxes lc rgb var",
		"plot 'file_data.dat' using 0:($2+0.5):2 with labels"
	};

	// create pipe to gnuplot
	FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");

	// send commands to gnuplot one by one
	int i;
	for (i = 0; i < NUM_COMMANDS; i++)
    	fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]); 

    fflush(gnuplotPipe);

	return 0;
}
