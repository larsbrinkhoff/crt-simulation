#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <netdb.h>
#include <poll.h>

static int sockfd;

int acc(void)
{
	struct sockaddr_in client;
	socklen_t len = sizeof client;
	return accept(sockfd, (void *)&client, &len);
}

int serve(int port)
{
	struct sockaddr_in server;
	int x;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		return -1;
	}

	x = 1;
	setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&x, sizeof x);
	
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	if(bind(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0){
		close(sockfd);
		return -1;
	}
	listen(sockfd, 5);
	return acc();
}

int transfer (int fd, void *buffer, int size)
{
  struct pollfd p;
  int n = 0;

  p.fd = fd;
  p.events = POLLIN|POLLERR;

  n = poll (&p, 1, 0);
  if (n > 0) {
    if (p.revents & POLLERR)
      return -1;
    if (p.revents & POLLHUP)
      return -1;
    if (p.revents & POLLIN)
      n = read (fd, buffer, size);
    if (n == 0)
      return -1;
  }

  return n;
}
