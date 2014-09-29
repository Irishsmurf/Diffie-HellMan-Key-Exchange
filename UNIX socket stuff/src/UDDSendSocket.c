/* compile using cc -o UDDSendSocket UDDSendSocket.c */
#include </usr/include/sys/types.h>
#include </usr/include/sys/socket.h>

#include </usr/include/sys/un.h>
#include <stdio.h>

#define DATA "The sea oh the sea, its a sure guarantee ..."

main(argc, argv)
int argc;
char *argv[];
{
	int sock;
	struct sockaddr_un name;

/* create socket on which to send */

	sock=socket(AF_UNIX, SOCK_DGRAM, 0);
	if(sock<0)
	{
		perror("opening datagram socket");
		exit(1);
	}
	
/* construct name of socket to send to. */

	name.sun_family=AF_UNIX;
	strcpy(name.sun_path, argv[1]);

/* send message */

	if(sendto(sock, DATA, sizeof(DATA), 0, (struct sockaddr *)&name, sizeof(struct sockaddr_un))<0)
	{
		perror("sending datagram message");
	}
	close(sock);
	exit(0);
}	
