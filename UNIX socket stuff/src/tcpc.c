/*
	example tcp based client.
*/

#include "inet.h"
#include <string.h>

main(argc, argv)
int argc;
char *argv[];
{
	int	sock, rc, bytes;
	struct sockaddr_in	serv_addr;
	struct hostent *host;
	char *msg = "hellothur", rmsg[10];  /* rmsg is big enough to hold 'bye' */

	if (argc < 2)
		{
		printf("\nusage: tcpc hostname\n\n");
		exit(-1);
		}

	printf("\ncreating socket...");
	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
		perror("Opening a socket");
		exit(-1);
		}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SRV_TCP_PORT);

	host = gethostbyname(argv[1]);
	if (host == NULL)
		{
		perror("gethostbyname");
		exit(-1);
		}
	bcopy(host->h_addr, (char *)&serv_addr.sin_addr, host->h_length);

	printf("\nconnecting to server...");
	if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		{
		perror("Connecting to a socket");
		exit(-1);
		}
	printf("done!");

	printf("\nsending msg to server...");
	rc = send(sock, msg, strlen(msg)+1, 0);

	printf("\nreading reply from server: ");
	bytes = recv(sock, rmsg, sizeof(rmsg), 0);
	printf("%s", rmsg);

	printf("\nclosing socket\n\n");
	close(sock);

	exit(0);
}
