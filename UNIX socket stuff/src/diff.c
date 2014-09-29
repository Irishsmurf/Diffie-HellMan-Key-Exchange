
/*
	example tcp based server.
*/

#include "inet.h"
#include "includes/miracl.h"
#include <string.h>
#include <stdlib.h>

time_t seed;
miracl *mip;

int server(int port)
{
	#ifndef MR_NOFULLWIDTH
	mip = mirsys(50, 0);
	#else
	mip = mirsys(50, MAXBASE);
	#endif
	int size = 1024;
	int n = 1024/8;
	big bigPrime, smallPrime,  bobPriv, bobPrime, bobKey, alicePrime;
	
	bigPrime 	= mirvar(0);
	smallPrime 	= mirvar(0);
	bobPriv 	= mirvar(0);
	bobPrime 	= mirvar(0);
	bobKey 		= mirvar(0);
	alicePrime 	= mirvar(0);
	
	time(&seed);
	irand((unsigned long)seed);

//Do Sockety Stuff


	int	srvsock, newsock, clilen, childpid, rc, bytes;
	struct sockaddr_in	cli_addr, serv_addr;
	char msg[n], rmsg[n];  /* msg is big enough to hold 'hello' */

	if ( (srvsock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Opening a TCP socket");
		return (-1);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family       = AF_INET;
	serv_addr.sin_port         = port;
	serv_addr.sin_addr.s_addr  = htonl(INADDR_ANY);

	if (bind(srvsock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("Binding to a socket");
		return (-1);
	}
	printf("Port ----> %d\n", serv_addr.sin_port);
	listen(srvsock, 5);

	clilen = sizeof(struct sockaddr_in);
	newsock = accept(srvsock, (struct sockaddr *) &cli_addr, &clilen);
	if (newsock < 0)
	{
		perror("Accepting a connection");
		return (-1);
	}

	//Recieve Big and Small prime
	recv(newsock, msg, n, 0);
	recv(newsock, rmsg, n,0);

	//------------CONVERT FROM BYTES TO BIG --------------
	bytes_to_big(n, msg, smallPrime);
	bytes_to_big(n, rmsg, bigPrime);

	//Compute Bob's private number
	bigbits(size, bobPriv);
	powmod(smallPrime, bobPriv, bigPrime, bobPrime);
	
	
	//Send Bob's Calculation	
	big_to_bytes(n, bobPrime, msg, 1); 
	send(newsock, msg, n, 0);
	
	//Recieve Alice's Calulated number
	recv(newsock, rmsg, n, 0);
	bytes_to_big(n, rmsg, alicePrime);
	
	//We have everything, do magic
		
	powmod(alicePrime, bobPriv, bigPrime, bobKey);

	//Close Sockets
	shutdown(newsock, SHUT_RDWR);			 
	shutdown(srvsock, SHUT_RDWR);				/* close the original socket */


	printf("Bob's Private Key:\n");
	cotnum(bobKey, stdout);

	printf("\n");
	return 0;
}

int client(int port, char *hostname)
{
	
	#ifndef MR_NOFULLWIDTH
	mip=mirsys(50,0);
	#else
	mip=mirsys(50,MAXBASE);
	#endif
	
	time(&seed);
	
	int size = 1024;
	int n = 1024/8;

	irand((unsigned int) seed);
	big bigPrime, smallPrime, alicePrime, alicePriv, aliceKey, bobPrime;

	bigPrime 	= mirvar(0);
	smallPrime 	= mirvar(0);
	alicePrime 	= mirvar(0);
	alicePriv 	= mirvar(0);
	aliceKey 	= mirvar(0);
	bobPrime	= mirvar(0);


	

	int sock, rc, bytes;
	struct sockaddr_in      serv_addr;
	struct hostent *host;

	char msg[n], rmsg[n];

	//Create Socket
	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Opening a socket");
		return (-1);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = port;

	host = gethostbyname(hostname);
	if (host == NULL)
	{
		perror("gethostbyname");
		return (-1);
	}
	bcopy(host->h_addr, (char *)&serv_addr.sin_addr, host->h_length);
	
	//Connect to Socket
	if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("Connecting to a socket");
		return (-1);
	}
	
	//Calculate Primes
	bigbits(size, smallPrime);
	nxprime(smallPrime, smallPrime);
	nxprime(smallPrime, bigPrime);

	big_to_bytes(n, smallPrime, msg, 1);
	big_to_bytes(n, bigPrime, rmsg, 1);

	//Create Alice's private key
	bigbits(n, alicePriv);

	//Create Alice's Calculation
	powmod(smallPrime, alicePriv, bigPrime, alicePrime);
	
	//Send Small prime and then large prime
	send(sock, msg, n, 0);
	send(sock, rmsg, n, 0);

	//Get Bob's mixture of the two.
	recv(sock, msg, n, 0);
	bytes_to_big(n, msg, bobPrime);

	//Send Alice's
	big_to_bytes(n, alicePrime, rmsg, 1);
	send(sock, rmsg, n, 0);

	//Everything done, compute key

	powmod(bobPrime, alicePriv, bigPrime, aliceKey);

	//Close Socket
	close(sock);

	
	printf("Alice's Private Key is :\n");
	cotnum(aliceKey, stdout);
	printf("\n");

	return (0);
}

int main(int argc, char *argv[])
{
	if(argc > 0)
	{
		if(*argv[1] == 'C' || *argv[1] == 'c')
		{
			if(argc < 3)
			{
				printf("A client needs to be supplied with a Host and Port -> %s hostname port.\n", argv[0]);
				return -1;
			}
			else
			{
				int port = atoi(argv[3]);
				client(port, argv[2]);
			}
		}
		else if(*argv[1] == 'S' || *argv[1] == 's')
		{
			if(argc < 2)
			{
				printf("Server needs to be supplied with a port -> %s port.\n", argv[0]);
				return -1;
			}
			else
			{
				int port = atoi(argv[2]);
				server(port);
			}
		}
		else
			printf("Must execute as %s S portnumber\nor\n%s C hostname portnumber.\n", argv[0], argv[0]);
	}
	else
		printf("Missing arguments.\n");
	
	return 0;
}



