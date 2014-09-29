/*
	example tcp based server.
*/

#include "inet.h"
#include "includes/miracl.h"
#include <string.h>
#include <stdlib.h>


int server()
{
	int	srvsock, newsock, clilen, childpid, rc, bytes;
	struct sockaddr_in	cli_addr, serv_addr;
	char msg[10], *rmsg = "bye bye";  /* msg is big enough to hold 'hello' */

	if ( (srvsock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
		perror("Opening a TCP socket");
		return (-1);
		}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family       = AF_INET;
	serv_addr.sin_port         = SRV_TCP_PORT;
	serv_addr.sin_addr.s_addr  = htonl(INADDR_ANY);

	if (bind(srvsock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		{
		perror("Binding to a socket");
		return (-1);
		}
	printf("Port ----> %d\n", serv_addr.sin_port);
	listen(srvsock, 5);

	printf("\nWaiting to accept a connection...");
	clilen = sizeof(struct sockaddr_in);
	newsock = accept(srvsock, (struct sockaddr *) &cli_addr, &clilen);
	if (newsock < 0)
	{
		perror("Accepting a connection");
		return (-1);
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

int client(int argc, char *argv[])
{
	
	int	sock, rc, bytes;
	struct sockaddr_in      serv_addr;
	struct hostent *host;
	char derp = argc;
	char *msg = "LOLWUT", rmsg[10];  /* rmsg is big enough to hold 'bye' */
	int port = atoi(argv[3]);       
	printf("\ncreating socket...");
	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Opening a socket");
		return (-1);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = SRV_TCP_PORT;

	host = gethostbyname(argv[1]);
	if (host == NULL)
	{
		perror("gethostbyname");
		return (-1);
	}
	bcopy(host->h_addr, (char *)&serv_addr.sin_addr, host->h_length);
	
	printf("\nconnecting to server...");
	if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("Connecting to a socket");
		return (-1);
	}
	printf("done!");

	printf("\nsending msg to server...");
	rc = send(sock, msg, strlen(msg)+1, 0);
	
	printf("\nreading reply from server: ");
	bytes = recv(sock, rmsg, sizeof(rmsg), 0);
	printf("%s", rmsg);

	printf("\nclosing socket\n\n");
	close(sock);
	return (0);
}

int main(int argc, char *argv[])
{
	time_t seed;
	time(&seed);
	big a;
	
	a = mirvar(0);
	bigbits(160, a);
	if(argc < 2)
		server();
	else 
		client(5, argv);
	return (0);
}



