/*
 *	ser4.c - (Topic 10, HX, 15/5/1995)
 *		Server for reversing strings, using TCP stream socket
 *		Server address is hard coded in the server and client
 *
 *	myftpd - (Assignment 2, Gil Hicks/Aaron Thomson, 21/10/2017)
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>


#ifdef _WIN32

#include <io.h>
#include <process.h>

inline FILE* popen(const char* command, const char* type) 
{
	return _popen(command, type);
}

inline void pclose(FILE* file) 
{ 
	_pclose(file); 
}

inline int open(const char* path, int oflag)
{
	return _open(path, oflag);
}

inline int open(const char* path, int oflag, mode_t mode)
{
	return _open(path, oflag, mode);
}

inline int close(int filedescriptor)
{
	return _close(filedescriptor);
}

#else
//Unix includes

#endif

#define BUF_SIZE	256
#define MAXF		10240

void kill_zombies ()
{
	pid_t pid = 1;

	while (pid > 0) 
	{
		pid = waitpid(-1, (int *)0, WNOHANG);
	}
}

void daemon_init(void)
{
	pid_t pid;
	struct sigaction act;

	if((pid = fork()) < 0)
	{
		perror("Fork");
		exit(1);
	}
	else if(pid > 0)
	{
		//Close the parent process
		exit(0);
	}

	//Child continues
	setsid(); 	//Become session leader
	chdir("/"); 	//Change working directory
	umask(0);	//Clear out file mode creation mask

	act.sa_handler = kill_zombies;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_NOCLDSTOP;
	sigaction(SIGCHLD, (struct sigaction *)&act, (struct sigaction *)0);
}

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

void serve_a_client(int sd)
{
	FILE *fp;
	int nr, nw;
	char buf[MAXF];
	char path[256];
	char filename[256];
	char cmdfull[256];
	char temp[256];
	char username[256];	
	char currentdirectory[256];
	char temp2[1035];
	char filesizestr[256];
	int ret_val;
	long filesize;
	strcpy(cmdfull, "cd && ");

	while(1) 
	{
		//Read data from the client
		if((nr = read(sd, buf, sizeof(buf))) <= 0)
		{
			//Connection broke down
			exit(0);
		}
		
		//Process data
		buf[nr] = '\0';
		if(strcmp(buf, "pwd") == 0)
		{	
			#ifdef _WIN32
				strcpy(buf, "cd");
			#endif
			strcat(strcpy(temp, cmdfull), buf);
			fp = popen(temp, "r");
			if (fp == NULL) 
			{
				printf("Failed to run command\n" );
			}
			memset(buf, 0, sizeof(buf));
			memset(temp2, 0, sizeof(temp2));
			while (fgets(temp2, sizeof(temp2)-1, fp) != NULL) 
			{
				strcat(buf, temp2);
			}
			pclose(fp);
			nw = write(sd, buf, strlen(buf));
			memset(buf, 0, sizeof(buf));
		}
		else if(strcmp(buf, "dir") == 0)
		{
			strcat(strcpy(temp, cmdfull), buf);
			fp = popen(temp, "r");
			if (fp == NULL) 
			{
				printf("Failed to run command\n" );
			}
			memset(buf, 0, sizeof(buf));
			memset(temp2, 0, sizeof(temp2));
			while (fgets(temp2, sizeof(temp2)-1, fp) != NULL) 
			{
				strcat(buf, temp2);
			}
			pclose(fp);
			nw = write(sd, buf, strlen(buf));
			memset(buf, 0, sizeof(buf));
			
		}
		else if(strncmp(buf, "cd", 2) == 0 )
		{	
			#ifdef _WIN32
				if (strcmp(buf, "cd") == 0)
				{
					strcpy(buf, "cd %userprofile%");
				}
			#endif
			strcat(strcpy(temp, cmdfull), buf);
			ret_val = system(temp);

			fp = popen(temp, "r");
			if (fp == NULL) 
			{
				printf("Error occured\n" );
			}
			fgets(buf, sizeof(buf)-1, fp);
			if(buf[strlen(buf) - 1] == '\n')
			{
				buf[strlen(buf) - 1] = '\0';
			}
			pclose(fp);

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
				strcpy(cmdfull, strcat(strcat(strcpy(temp, "cd "), currentdirectory), " && "));
				pclose(fp);
			}
			nw = write(sd, buf, strlen(buf));
			memset(buf, 0, sizeof(buf));
		}
		else if(strcmp(buf, "kill") == 0) 
		{
			kill(getppid(), SIGTERM);
		}
		else if(strncmp(buf, "get", 3) == 0)
		{
			memset(filename, 0, sizeof(filename));
			for(int i = 0; i < strlen(buf) - 3; i++)
			{
				filename[i] = buf[i + 4];
			}
			strcat(strcat(strcpy(path, currentdirectory), "/"), filename); 	
			memset(buf, 0, sizeof(buf));
			filesize = cpsrc(path, buf);
			sprintf(filesizestr, "%lu", filesize);
			nw = write(sd, filesizestr, strlen(filesizestr));
			nw = write(sd, buf, filesize);	
			memset(buf, 0, sizeof(buf));	
		}
		else if(strncmp(buf, "put", 3) == 0)
		{	
			memset(filename, 0, sizeof(filename));
			for(int i = 0; i < strlen(buf) - 3; i++)
			{
				filename[i] = buf[i + 4];
			}
			strcat(strcat(strcpy(path, currentdirectory), "/"), filename); 
			nw = write(sd, filename, strlen(filename));
			memset(buf, 0, sizeof(buf));
			nr = read(sd, buf, MAXF);
			filesize = atol(buf);
			memset(buf, 0, sizeof(buf));
			nr = read(sd, buf, MAXF);
			cpdest(buf, path, filesize);
			memset(buf, 0, sizeof(buf));			
		}
	}
}

int main(int argc, char *argv[])
{
	int sd, nsd, n, spn, sip, cli_addr_len;
	char sipstr[16];
	pid_t pid;
	struct sockaddr_in ser_addr, cli_addr;
	
	//Get port number from command line or user input
	if(argc == 3)
	{
		spn = argv[1];

		argv[2][strlen(argv[2])] = '.';
		sip = conviptodec(argv[2]);
	}
	else
	{
		printf("Enter a port number: ");
		scanf("%d", &spn);

		printf("Enter an ip address: ");
		scanf("%s", sipstr);
		sipstr[strlen(sipstr)] = '.';

		sip = conviptodec(sipstr);
	}
	
	//Turn the program into a daemon_init
	daemon_init();
	
	//Set up listening socket sd
	if((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Server:socket");
		exit(1);
	}
	
	//Build server Internet socket address
	bzero((char *)&ser_addr, sizeof(ser_addr));
	
	ser_addr.sin_family = AF_INET;

	ser_addr.sin_port = htons((uint16_t)spn);

	ser_addr.sin_addr.s_addr = htonl((uint32_t)sip);
	
	//bind server address to socket sd
	if(bind(sd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) < 0)
	{
		perror("server bind");
		exit(1);
	}
	
	//Become a listening socket
	listen(sd, 5);
	
	while(1) {
		//Wait to accept a client request for connection
		cli_addr_len = sizeof(cli_addr);
		
		nsd = accept(sd, (struct sockaddr *)&cli_addr, &cli_addr_len);
		
		if(nsd < 0)
		{
			if(errno == EINTR) //If interrupted by SIGCHILD
				continue;

			perror("server:accept");
			exit(1);
		}
		
		//Create a child process to handle this client
		if((pid = fork()) < 0)
		{
			perror("Fork");
			exit(1);
		}
		else if(pid > 0)
		{
			close(nsd);
			continue; //Parent to wait for next client
		}
		
		//Now in child, server the current client
		close(sd);
		
		serve_a_client(nsd);
	}

	exit(0);
}
