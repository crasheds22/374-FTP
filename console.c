#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main( int argc, char *argv[] )
{
	FILE *fp;
	char path[1035];
	char command[256];
	char cmdfull[256];
	char temp[256];
	char cmdcd[3];
	char homedirectory[256];
  	int exit = 1;
	int nr;
	int ret_val;
	
	getcwd(homedirectory, sizeof(homedirectory));
	printf("%s\n", homedirectory);
	strcpy(cmdfull, "");

	do
	{
		printf("enter comand:");
		fgets(command, 256, stdin);
		
		nr = strlen(command);
		if(command[nr - 1] == '\n')
		{
			command[nr - 1] = '\0';
			--nr;
		}

		cmdcd[0] = command[0];
		cmdcd[1] = command[1];
		cmdcd[2] = '\0';

		if(strcmp(command, "pwd") == 0 || strcmp(command, "dir") == 0)
		{
			strcat(strcpy(temp, cmdfull), command);
			printf("%s\n", temp);
			//system(temp);
			fp = popen(temp, "r");
			if (fp == NULL) 
			{
				printf("Failed to run command\n" );
			}

			/* Read the output a line at a time - output it. */
			while (fgets(path, sizeof(path)-1, fp) != NULL) 
			{
				printf("%s", path);
			}

			/* close */
			pclose(fp);

		}
		else if(strcmp(cmdcd, "cd") == 0 )
		{	
			ret_val = system(command);
			if(ret_val == 0)
			{
				strcat(strcpy(cmdfull, command), " && ");
			}
			//needed for server
			//fp = popen(command, "r");
			//if (fp == NULL) 
			//{
			//	printf("Failed to run command\n" );
			//}

			/* Read the output a line at a time - output it. */
			//while (fgets(path, sizeof(path)-1, fp) != NULL) 
			//{
			//	printf("%s", path);
			//}

			/* close */
			//pclose(fp);
		}
		else if(strcmp(command, "quit") == 0)
		{
			printf("goodbye\n" );
			exit = 0;
		}
		else
		{
			printf("%s\n", command);
			printf("unknown command\n");
		}
	}while(exit == 1);		

	return 0;
}
