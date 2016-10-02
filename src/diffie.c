/* compile using cc -o DIDReadSocket DIDReadSocket.c */

#include <strings.h>
#include <stdlib.h>
#include <netdb.h>
#include </usr/include/sys/types.h>
#include </usr/include/sys/socket.h>
#include </usr/include/netinet/in.h>
#include <stdio.h>

#define DATA "The sea oh the sea, I am an Internet datagram..."

int sendSock(argc, argv)
int argc;
char *argv[];
{
  int sock;
  struct sockaddr_in name;
  struct hostent *hp, *gethostbyname();

  sock=socket(AF_INET, SOCK_DGRAM, 0);
  if(sock<0) {
    perror("opening datagram socket");
    return 1;
  }

  /* construct name, of socket to send to, gethostbyname() returns struct including network address of host, port# taken from command line */
  hp=gethostbyname(argv[1]);
  if (hp==0) {
    fprintf(stderr, "%s: unknown host", argv[1]);
    return 2;
  }
  bcopy((char *)hp->h_addr, (char *)&name.sin_addr, hp->h_length);
  name.sin_family=AF_INET;
  name.sin_port=htons(atoi(argv[2]));

/* send message */
  if(sendto(sock, DATA, sizeof DATA, 0,
      (struct sockaddr *)&name, sizeof name) < 0){
    perror("sending datagram message");
    close(sock);
    return 0;
  }

/* in.h defines sockaddr_in as a struct */
/* program to create datagram socket in Internet domain, binds a name to it & reads from it */

int readSock() {
  int sock, length;
  struct sockaddr_in name;
  char buf[1024];

  /* create socket from which to read */
  sock=socket(AF_INET, SOCK_DGRAM, 0);
  if(sock<0) {
    perror("opening datagram socket");
    return 1;
  }

  /* create name with wildcards */
  name.sin_family=AF_INET;
  name.sin_addr.s_addr=INADDR_ANY;
  name.sin_port= 0;
  if(bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
    perror("binding datagram socket");
    return 1;
  }

  /* find assigned port value and print it out */
  length=sizeof(name);
  if (getsockname(sock, (struct sockaddr *)&name, &length)<0) {
    perror("getting socket name");
    return 1;
  }
  printf("Socket port #%d\n", ntohs(name.sin_port));

  /* read from socket */
  if (read(sock, buf, 1024)<0) {
    perror("receiving datagram packet");
  }
  printf("----->%s\n", buf);
  close(sock);
  return 0;
}


int main(int argi, char * argv[]) {
  char* hostname;
  int port;
  if(argi > 2) {
    hostname = argv[1];
    port = atoi(argv[2]);
    sendSock(hostname, port);
    //	readSocket();
  }
  else {
    readSock();
  //	sendSocket(hostname, port);
  }
  printf("derp\nHostname = %s\nPort = %d\n", hostname, port);
  return 0;
}
