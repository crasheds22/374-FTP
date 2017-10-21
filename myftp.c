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
#include <netintet/in.h> /* struct sockaddr_in, htons, htonl */
 
#include <string.h>
#include <stdio.h>
#inclide <stdlib.h>

#define BUF_SIZE 		256

int conviptodec(char *addr)
{
	//convert the ip string to a decimal number and return that number
}

main(int argc, char *argv[])
{
	int sd, n, nr, nw, pn, ip, i = 0;
	char buf[BUF_SIZE]; /*ipstr usrnm*/
	struct sockaddr_in ser_addr;
	
	//If no port number, username or ip provided
	if(argc == 1) 
	{
		//Prompt for port number
		printf("Port number: ");
		scanf("%d", &pn);
		//Prompt for ip address in A.B.C.D form -> ipstr
		//ip = conviptodec(ipstr)
		//Prompt for desired username
	}
	else if (argc == 4)
	{
		pn = argv[1];
		//ip = conviptodec(argv[2]);
		//usrnm = argv[3];
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
		
		
		if(nr > 0)
		{
			//Write to the socket from the buffer
			nw = write(sd, buf, nr);
			//Read from the socket to the buffer
			nr = read(sd, buf, BUF_SIZE);
			//Add in null terminator
			buf[nr] = '\0';
			//Print message to screen
			printf("Server output[%d]: %s\n", i, buf);
		}
	}
}
