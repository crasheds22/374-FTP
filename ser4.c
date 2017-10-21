/*
 *  ser4.c  - 	(Topic 10, HX 15/5/1995)
 *		server for reversing strings, using TCP stream socket.
 *		Server machine's address (134.115.64.72) is hard 
 *		coded in the server & client. 
 */

#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>      /* struct sockaddr_in, htons, htonl */
#include  <string.h>
#include  <sys/wait.h>
#include  <signal.h>


#define   BUFSIZE         256
#define   SERV_INET_NO    2255722821   /* 134.115.153.69 */
#define   SERV_TCP_PORT   42000        /* server port no */

void kill_zombies ()
{
	pid_t pid = 1;
	int chainsaw;

	while (pid > 0) {
		pid = waitpid(0, (int *)0, WNOHANG);
	}
}

void daemon_init(void)
{
	 pid_t   pid;
	struct sigaction act;

        if ( (pid = fork()) < 0) {
             perror("fork"); exit(1); 
        } else if (pid > 0) 
             exit(0);        /* parent goes bye-bye */
        /* child continues */
        setsid();               /* become session leader */
        chdir("/");             /* change working directory */
        umask(0);               /* clear our file mode creation mask */

	act.sa_handler = kill_zombies;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_NOCLDSTOP;
	sigaction(SIGCHLD, (struct sigaction *)&act, (struct sigaction *)0);
}

void reverse(char *s)
{   char c;  int i, j=strlen(s); 

    for (i=0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i]; s[i] = s[j]; s[j] = c;
    }
}

void serve_a_client(int sd)
{   
    int nr, nw;
    char buf[BUFSIZE];

    while (1){
         /* read data from client */
         if ((nr = read(sd, buf, sizeof(buf))) <= 0) 
             exit(0);   /* connection broken down */

         /* process data */
         buf[nr] = '\0';
	 reverse(buf);

         /* send results to client */
         nw = write(sd, buf, nr);
    } 
}


main()
{
     int sd, nsd, n, cli_addrlen;  pid_t pid;
     struct sockaddr_in ser_addr, cli_addr; 
     
     /* turn the program into a daemon */
     daemon_init();  

     /* set up listening socket sd */
     if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
           perror("server:socket"); exit(1);
     } 

     /* build server Internet socket address */
     bzero((char *)&ser_addr, sizeof(ser_addr));
     ser_addr.sin_family = AF_INET;
     ser_addr.sin_port = htons(SERV_TCP_PORT);
     ser_addr.sin_addr.s_addr = htonl(SERV_INET_NO); 

     /* bind server address to socket sd */
     if (bind(sd, (struct sockaddr *) &ser_addr, sizeof(ser_addr))<0){
           perror("server bind"); exit(1);
     }

     /* become a listening socket */
     listen(sd, 5);

     while (1) {

          /* wait to accept a client request for connection */
          cli_addrlen = sizeof(cli_addr);
          nsd = accept(sd, (struct sockaddr *) &cli_addr, &cli_addrlen);
          if (nsd < 0) {
               perror("server:accept"); exit(1);
          }

          /* create a child process to handle this client */
          if ((pid=fork()) <0) {
              perror("fork"); exit(1);
          } else if (pid > 0) { 
              close(nsd);
              continue; /* parent to wait for next client */
          }

          /* now in child, serve the current client */
          close(sd); 
	  serve_a_client(nsd);
     }
}
