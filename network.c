#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <netdb.h>
#include <poll.h>

int serve(int port)
{
	int sockfd, confd;
	socklen_t len;
	struct sockaddr_in server, client;
	int x;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		perror("error: socket");
		return -1;
	}

	x = 1;
	setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&x, sizeof x);
	
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	if(bind(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0){
		perror("error: bind");
		return -1;
	}
	listen(sockfd, 5);
	len = sizeof(client);
	confd = accept(sockfd, (struct sockaddr*)&client, &len);
	return confd;
}

int transfer (int fd, void *buffer, int size)
{
  struct pollfd p;
  int n;

  p.fd = fd;
  p.events = POLLIN;

  n = poll (&p, 1, 0);
  if (n > 0) {
    n = read (fd, buffer, size);
  }

  return n;
}
