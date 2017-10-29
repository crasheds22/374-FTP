#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define MAXN	256
#define MAXF	4096

//Copy files from src to destination
void cp(char *src, char *dest)
{
	int to, from;
	char buf[MAXF];
	ssize_t nread;

	if((to = open(dest, O_WRONLY | O_CREAT, 0777)) < 0)
	{
		perror("destination");
		exit(1);
	}

	if((from = open(src, O_RDONLY)) < 0)
	{
		perror("source");
		exit(1);
	}

	while((nread = read(from, buf, MAXF)) > 0)
		write(to, buf, nread);

	close(to);
	close(from);
	
	return;
}

int main()
{
	char source[MAXN], d[MAXN];

	printf("Enter source file: \n");
	fgets(source, MAXN, stdin);

	if(source[strlen(source) - 1] == '\n')
		source[strlen(source) - 1] = '\0';

	printf("Enter destination file: \n");
	fgets(d, MAXN, stdin);

	if(d[strlen(d) - 1] == '\n')
		d[strlen(d) - 1] = '\0';

	cp(source, d);
	//cp("myftp.c", "fix");	

	exit(0);
}


