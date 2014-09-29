/* compile using cc -o IDSSendSocket  IDSSendSocket.c


/* internet domain stream connection */
/* invoke as IDSSendSocket hostname portnumber */


#include </usr/include/sys/types.h>
#include </usr/include/sys/socket.h>
#include </usr/include/netinet/in.h>
#include <stdio.h>
#include <netdb.h>


#define DATA "the sea is awash with internet domain stream packets..."

main(argc, argv)
int argc;
char *argv[];
{
	int sock;
	struct sockaddr_in server;
	struct hostent *hp, *gethostbyname();
	char buf[1024];

	sock=socket(AF_INET, SOCK_STREAM, 0);
	if(sock<0)
	{
		perror("opening stream socket");
		exit(1);
	}
	server.sin_family=AF_INET;
	hp=gethostbyname(argv[1]);
	if (hp==0)
	{
		fprintf(stderr, "%s: unknown host", argv[1]);
		exit(2);
	}
	bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	server.sin_port=htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr *)&server, sizeof server)<0)
	{
		perror("connecting stream socket");
		exit(1);
	}
for(;;)
{
	if  (write(sock, DATA, sizeof DATA)<0) perror("writing on stream socket");
}
	close(sock);
	exit(0);
}

