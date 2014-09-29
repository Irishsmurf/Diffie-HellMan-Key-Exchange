/* compile using cc  -o DIDSendSocket DIDSendSocket.c 

/* sending an internet domain datagram */
/* address on command line please */


#include <netdb.h>
#include </usr/include/sys/types.h>
#include </usr/include/sys/socket.h>
#include </usr/include/netinet/in.h>
#include <stdio.h>



#define DATA "The sea oh the sea, I am an Internet datagram..."

main(argc, argv)
int argc;
char *argv[];
{
	int sock;
	struct sockaddr_in name;
	struct hostent *hp, *gethostbyname();

	sock=socket(AF_INET, SOCK_DGRAM, 0);
	if(sock<0)
	{
		perror("opening datagram socket");
		exit(1);
	}

/* construct name, of socket to send to, gethostbyname() returns struct including network address of host, port# taken from command line */

	hp=gethostbyname(argv[1]);
	if (hp==0)
	{
		fprintf(stderr, "%s: unknown host", argv[1]);
		exit(2);
	}
	bcopy((char *)hp->h_addr, (char *)&name.sin_addr, hp->h_length);
	name.sin_family=AF_INET;
	name.sin_port=htons(atoi(argv[2]));

/* send message */
	
	if(sendto(sock, DATA, sizeof DATA, 0, (struct sockaddr *)&name, sizeof name)<0)
		perror("sending datagram message");
	close(sock);
	exit(0);
}
