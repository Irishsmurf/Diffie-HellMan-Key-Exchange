/* compile using cc -o DIDReadSocket DIDReadSocket.c */


#include </usr/include/sys/types.h>
#include </usr/include/sys/socket.h>
#include </usr/include/netinet/in.h>
#include <stdio.h>

/* in.h defines sockaddr_in as a struct */

/* program to create datagram socket in Internet domain, binds a name to it & reads from it */

main()
{
	int sock, length;
	struct sockaddr_in name;
	char buf[1024];

/* create socket from which to read */

	sock=socket(AF_INET, SOCK_DGRAM, 0);
	if(sock<0)
	{
		perror("opening datagram socket");
		exit(1);
	}

/* create name with wildcards */

	name.sin_family=AF_INET;
	name.sin_addr.s_addr=INADDR_ANY;
	name.sin_port=0;
	if(bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0)
	{
		perror("binding datagram socket");
		exit(1);
	}

/* find assigned port value and print it out */

	length=sizeof(name);
	if (getsockname(sock, (struct sockaddr *)&name, &length)<0)
	{
		perror("getting socket name");
		exit(1);
	}
	printf("Socket port #%d\n", ntohs(name.sin_port));

/* read from socket */

	if (read(sock, buf, 1024)<0) perror("receiving datagram packet");
	printf("----->%s\n", buf);
	close(sock);
	exit(0);
}
