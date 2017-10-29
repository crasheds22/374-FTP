#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main( int argc, char *argv[] )
{
	FILE *fp;
	char path[1035];
	char command[256];
	char cmdfull[256];
	char temp[256];
	char cmd[4];
	char currentdirectory[256];
  	int exit = 1;
	int ret_val;
	//strcpy(currentdirectory, "/home/ict374/ass2");
	//getcwd(homedirectory, sizeof(homedirectory));
	strcpy(cmdfull, "cd && ");	

	do
	{
		printf("enter comand:");
		fgets(command, 256, stdin);
		
		if(command[strlen(command) - 1] == '\n')
		{
			command[strlen(command) - 1] = '\0';
		}

		if(strcmp(command, "pwd") == 0)
		{
			#ifdef _WIN32
				strcpy(command, "cd");
			#endif
			strcat(strcpy(temp, cmdfull), command);
			
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
		else if(strcmp(command, "dir") == 0)
		{
			strcat(strcpy(temp, cmdfull), command);
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
		else if(strncmp(command, "cd", 2) == 0 )
		{	
			#ifdef _WIN32
				if (strcmp(command, "cd") == 0)
				{
					strcpy(command, "cd %userprofile%");
					printf("it entered here\n");
				}
			#endif
			strcat(strcpy(temp, cmdfull), command);
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
				fgets(path, sizeof(path)-1, fp);
				if(path[strlen(path) - 1] == '\n')
				{
					path[strlen(path) - 1] = '\0';
				}
				strcpy(currentdirectory, path);
				strcpy(cmdfull, strcat(strcat(strcpy(temp, "cd "), currentdirectory), " && "));
			}
			//else
			//{
				//needed for server
				//fp = popen(command, "r");
				//if (fp == NULL) 
				//{
				//	printf("Failed to run command\n" );
				//}

				/* Read the output a line at a time - output it. */
				//while (fgets(path, sizeof(path)-1, fp) != NULL) 
				//{
				//	send to client
				//}

				/* close */
				//pclose(fp);
			//}
			
		}
		else if(strncmp(command, "lcd", 3) == 0 )
		{	
			for(int i = 0; i < sizeof(command) - 1; i++)
			{
				temp[i] = command[i+1];
			}
			printf("%s\n", temp);
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
