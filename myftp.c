/*
 *	ser4.c - (Topic 10, HX 15/5/1995)
 *		Client for reversing reversing strings, using TCP socket
 *		The server address is hard coded
 *
 *	mftp.c - (Assignment 2, Gil Hicks/Aaron Thomson, 21/10/2017)
 *
 */
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> /* struct sockaddr_in, htons, htonl */
 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE 	256
#define MAXF		10240

#ifdef _WIN32

inline FILE* popen(const char* command, const char* type) 
{
   return _popen(command, type);
}
inline void pclose(FILE* file) 
{ 
   _pclose(file); 
}

#endif

unsigned long conviptodec(char addr[])
{
	//convert the ip string to a decimal number and return that number
	char *piece;
	char temp[4][4];

	unsigned long full[4];
	unsigned long ret = 0;
	int i, j = 0, k = 0, m = 0;

	for(i = 0; i < strlen(addr); i++)
	{
		if(addr[i] == '.')
		{
			for(j = m; j <= i; j++)
			{
				if(j == i)
					temp[k][j - m] = '\0';
				else
					temp[k][j - m] = addr[j];
			}
 			k++;
			m = i + 1;
		}
	}

	for(m = 0; m < 4; m++)
	{
		full[m] = atoi(temp[m]);
	}

	ret += full[0] * (256 * 256 * 256);
	ret += full[1] * (256 * 256);
	ret += full[2] * 256;
	ret += full[3];

	return ret;
}

//Copy from buf to dest
void cpdest(char buf[], char dest[], long filesize)
{
	FILE *f = fopen(dest, "wb");
	fwrite(buf, filesize, 1, f);
	fclose(f);
}

//Copy files from src to buf
int cpsrc(char src[], char buf[])
{
	long fsize;	
	FILE *f =fopen(src, "rb");
	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	fread(buf, fsize, 1, f);
	fclose(f);
	buf[fsize] = 0;
	return(fsize);
}

int main(int argc, char *argv[])
{
	FILE *fp;
	int sd, n, nr, nw, pn, i = 0;
	int filenamepoint;
	unsigned long ip;
	char path[256];
	char filename[256];
	char buf[MAXF], ipstr[16]; /*usrnm*/
	struct sockaddr_in ser_addr;
	char cmdfull[256];
	char temp[256];	
	char currentdirectory[256];
	char temp2[1035];
	long filesize;
	char filesizestr[256];
	int ret_val;

	fp = popen("cd && pwd", "r"); 
	fgets(temp2, sizeof(temp2)-1, fp);
	if(temp2[strlen(temp2) - 1] == '\n')
	{
		temp2[strlen(temp2) - 1] = '\0';
	}
	strcpy(currentdirectory, temp2);
	strcpy(cmdfull, strcat(strcat(strcpy(temp, "cd "), currentdirectory), " && \0"));
	memset(buf, 0, sizeof(buf));
	memset(temp2, 0, sizeof(temp2));

	//If no port number, username or ip provided
	if (argc == 3)
	{
		pn = argv[1];

		argv[2][strlen(argv[2])] = '.';
		ip = conviptodec(argv[2]);
		//usrnm = argv[3];
	}
	else
	{
		//Prompt for port number
		printf("Port number: \n");
		fgets(temp, sizeof(temp), stdin);
		pn = atoi(temp);

		//Prompt for ip address in A.B.C.D form -> ipstr
		printf("IP number (in the form: A.B.C.D): \n");
		fgets(ipstr, 16, stdin);
		ipstr[strlen(ipstr)] = '.';

		ip = conviptodec(ipstr);

		//Prompt for desired username
	}

	/*Get host address and build a server socket address */
	bzero((char *)&ser_addr, sizeof(ser_addr));

	ser_addr.sin_family = AF_INET;
	//Port number
	ser_addr.sin_port = htons((uint16_t)pn);
	//IP address
	ser_addr.sin_addr.s_addr = htonl((uint32_t)ip);

	/*Create TCP socket and connect socket to server adddress */
	sd = socket(PF_INET, SOCK_STREAM, 0);

	if(connect(sd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) < 0)
	{
		perror("Client connect");
		exit(1);
	}

	while(++i)
	{
		//Prompt user for input
		printf("Client input[%d]: ", i);

		//Read in a line
		fgets(buf, BUF_SIZE, stdin);

		//Get the length of the input
		nr = strlen(buf);

		//Check for new line char in the string
		if(buf[nr - 1] == '\n')
		{
			buf[nr - 1] = '\0';
			--nr;
		}

		//Check for exit message
		if(strcmp(buf, "quit") == 0)
		{
			printf("Bye from the client\n");
			exit(0);
		}
		
		if(strcmp(buf, "kill") == 0)
		{
			nw = write(sd, buf, nr);

			printf("Bye from the client\n");
			exit(0);
		}
		
		if(strcmp(buf, "lpwd") == 0)
		{	
			strcpy(buf, "pwd");
			#ifdef _WIN32
				strcpy(buf, "cd");
			#endif
			strcat(strcpy(temp, cmdfull), buf);
			
			//system(temp);
			fp = popen(temp, "r");
			if (fp == NULL) 
			{
				printf("Failed to run command\n" );
			}

			/* Read the output a line at a time - output it. */
			while (fgets(temp2, sizeof(temp2)-1, fp) != NULL) 
			{
				printf("%s", temp2);
			}

			/* close */
			pclose(fp);
			memset(buf, 0, sizeof(buf));

		}
		else if(strcmp(buf, "ldir") == 0)
		{
			strcpy(buf, "dir");
			strcat(strcpy(temp, cmdfull), buf);
			//system(temp);
			fp = popen(temp, "r");
			if (fp == NULL) 
			{
				printf("Failed to run command\n" );
			}

			/* Read the output a line at a time - output it. */
			while (fgets(temp2, sizeof(temp2)-1, fp) != NULL) 
			{
				printf("%s", temp2);
			}

			/* close */
			pclose(fp);
			memset(buf, 0, sizeof(buf));
		}
		else if(strncmp(buf, "lcd", 3) == 0 )
		{	
			for(int i = 0; i < strlen(buf); i++)
			{
				buf[i] = buf[i+1];
			}
			#ifdef _WIN32
				if (strcmp(buf, "cd") == 0)
				{
					strcpy(buf, "cd %userprofile%");
				}
			#endif
			strcat(strcpy(temp, cmdfull), buf);
			ret_val = system(temp);
			if(ret_val == 0)
			{
				#ifdef _WIN32
					strcat(temp, " && cd");
				#else			
					strcat(temp, " && pwd");
				#endif
				
				fp = popen(temp, "r");
				if (fp == NULL) 
				{
					printf("Error occured\n" );
				}
				fgets(temp2, sizeof(temp2)-1, fp);
				if(temp2[strlen(temp2) - 1] == '\n')
				{
					temp2[strlen(temp2) - 1] = '\0';
				}
				strcpy(currentdirectory, temp2);
				strcpy(cmdfull, strcat(strcat(strcpy(temp, "cd "), currentdirectory), " && \0"));
				pclose(fp);
			}
			memset(buf, 0, sizeof(buf));
		}
		else if(strcmp(buf, "pwd") == 0)
		{
			nw = write(sd, buf, nr);
			memset(buf, 0, sizeof(buf));
			nr = read(sd, buf, MAXF);
			buf[strlen(buf)] = '\0';
			printf("%s", buf);
			memset(buf, 0, sizeof(buf));
		}
		else if(strcmp(buf, "dir") == 0)
		{
			nw = write(sd, buf, nr);
			memset(buf, 0, sizeof(buf));
			nr = read(sd, buf, MAXF);
			buf[strlen(buf)] = '\0';
			printf("%s", buf);
			memset(buf, 0, sizeof(buf));	
		}
		else if(strncmp(buf, "cd", 2) == 0)
		{
			strcpy(temp, buf);
			nw = write(sd, buf, nr);
			memset(buf, 0, sizeof(buf));
			nr = read(sd, buf, MAXF);
			if(strcmp(buf, temp) != 0)
			{
				buf[strlen(buf)] = '\0';
				printf("%s", buf);
			}
			memset(buf, 0, sizeof(buf));
		}
		else if(strncmp(buf, "get ", 4) == 0)
		{
			memset(filename, 0, sizeof(filename));
			for(int i = 0; i < strlen(buf) - 3; i++)
			{
				filename[i] = buf[i + 4];
			}
			strcat(strcat(strcpy(path, currentdirectory), "/"), filename); 	
			
			nw = write(sd, buf, nr);
			memset(buf, 0, sizeof(buf));
			nr = read(sd, buf, MAXF);
			filesize = atol(buf);
			memset(buf, 0, sizeof(buf));
			nr = read(sd, buf, MAXF);
			cpdest(buf, path, filesize);
			memset(buf, 0, sizeof(buf));
		}
		else if(strncmp(buf, "put ", 4) == 0)
		{
			memset(filename, 0, sizeof(filename));
			nw = write(sd, buf, strlen(buf));
			memset(buf, 0, sizeof(buf));
			nr = read(sd, buf, MAXF);
			strcpy(filename, buf);
			strcat(strcat(strcpy(path, currentdirectory), "/"), filename); 
			memset(buf, 0, sizeof(buf));
			filesize = cpsrc(path, buf);
			sprintf(filesizestr, "%lu", filesize);
			nw = write(sd, filesizestr, strlen(filesizestr));
			nw = write(sd, buf, filesize);	
			memset(buf, 0, sizeof(buf));	
		}	
		else
		{
			printf("Invalid command\n");
		}

	}
}
