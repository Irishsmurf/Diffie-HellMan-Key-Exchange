#include </usr/include/sys/types.h>
#include </usr/include/sys/socket.h>
#include </usr/include/sys/un.h>
#include <stdio.h>
/* un.h defines sockaddr_un as a struct. cf P260 manual */

#define NAME "socket"

/* program creates UNIX domain datagram socket, binds name to it, then reads from it. */

main()
{
	int sock, length;
	struct sockaddr_un name;
	char buf[1024];

/* create socket from which to read */

	sock=socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock<0) {perror("ERROR opening datagram socket");
		exit(1);
	}
	
/* create name. */

	name.sun_family=AF_UNIX;
	strcpy(name.sun_path, NAME);
	if(bind(sock, (struct sockaddr *)&name, strlen(NAME)+3) < 0)
	{
		perror("binding name to datagram socket");
		exit(1);
	}
	printf("socket ---> %s\n", NAME);

/* read from the socket */

	if(read(sock, buf, 1024) < 0) perror("receiving datagram packet");
	printf(" ---> %s\n", buf);
	close(sock);
	unlink(NAME);
	exit(0);

}
	