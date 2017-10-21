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
#include <sys/wait.h>
#include <netinet/in.h> 

#include <string.h>
#include <signal.h>
#include <stdlib.h>

#BUF_SIZE	256

void daemon_init(void)
{
	pid_t pid;
	
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
	chdir("/"); //Change working directory
	umask(0);	//Clear out file mode creation mask
}

void reverse(char *s)
{
	char c;
	int i, j = strlen(s);
	
	for(i = 0, j = strlen(s) - 1; i < j; i++, j--)
	{
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

void serve_a_client(int sd)
{
	int nr, nw;
	char buf[BUF_SIZE];
	
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
		reverse(buf);
		
		//Send results to the client
		nw = write(sd, buf, nr);
	}
}

main()
{
	int sd, nsd, n, spn, sip, cli_addr_len;
	pid_t, pid;
	struct sockaddr_in ser_addr, cli_addr;
	
	//Turn the program into a daemon_init
	daemon_init();
	
	//Set up listening socket sd
	if((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Server:socket");
		exit(1);
	}
	
	//Get the port number and ip address for the server to use
	//port number = spn
	//ip-address = sip
	
	//Build server Internet socket address
	bzero((char *)&ser_addr, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(spn);
	ser_addr.sin_addr.s_addr = htonl(sip);
	
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
}
