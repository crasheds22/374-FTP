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

#define BUF_SIZE 	256

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

int main(int argc, char *argv[])
{
	int sd, n, nr, nw, pn, i = 0;
	unsigned long ip;
	char buf[BUF_SIZE], ipstr[16]; /*usrnm*/
	struct sockaddr_in ser_addr;

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
		scanf("%d", &pn);

		//Prompt for ip address in A.B.C.D form -> ipstr
		printf("IP number (in the form: A.B.C.D): \n");
		scanf("%s", ipstr);
		ipstr[strlen(ipstr)] = '.';

		ip = conviptodec(ipstr);

		//Prompt for desired username
	}

	printf("ip in decimal form = %lu \n", ip);

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

		if(strncmp(buf, "get", 3) == 0)
		{
			nw = write(sd, buf, nr);


		}
		else if(strncmp(buf, "put", 3) == 0)
		{
			nw = write(sd, buf, nr);


		}	
		else if(nr > 0)
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
