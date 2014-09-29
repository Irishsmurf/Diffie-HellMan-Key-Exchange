/*
	example tcp based server.
*/

#include "inet.h"
#include <string.h>

main(argc, argv)
int argc;
char *argv[];
{
	int	srvsock, newsock, clilen, childpid, rc, bytes;
	struct sockaddr_in	cli_addr, serv_addr;
	char msg[10], *rmsg = "bye bye";  /* msg is big enough to hold 'hello' */

	if ( (srvsock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
		perror("Opening a TCP socket");
		exit(-1);
		}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family       = AF_INET;
	serv_addr.sin_port         = htons(SRV_TCP_PORT);
	serv_addr.sin_addr.s_addr  = htonl(INADDR_ANY);

	if (bind(srvsock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		{
		perror("Binding to a socket");
		exit(-1);
		}

	listen(srvsock, 5);

	printf("\nWaiting to accept a connection...");
	clilen = sizeof(struct sockaddr_in);
	newsock = accept(srvsock, (struct sockaddr *) &cli_addr, &clilen);
	if (newsock < 0)
		{
		perror("Accepting a connection");
		exit(-1);
		}
	printf("done!");

	printf("\nreading request from client: ");
	bytes = recv(newsock, msg, sizeof(msg), 0);
	printf("%s", msg);

	printf("\nsending reply to client...");
	rc = send(newsock, rmsg, strlen(rmsg)+1, 0);

	printf("\nclosing all sockets\n\n");
	shutdown(newsock, SHUT_RDWR);			 
	shutdown(srvsock, SHUT_RDWR);				/* close the original socket */
}
