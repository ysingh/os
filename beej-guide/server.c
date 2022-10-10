#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <wait.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MY_PORT "3490"
#define BACKLOG 10
#define BUFLEN 2048

int main(void) {
	struct sockaddr_storage their_addr;
  int sockfd;
	struct addrinfo hints;
	struct addrinfo* serverinfo;
	struct addrinfo* p;
	char buf[BUFLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; //use my IP

	int getaddrinfores = getaddrinfo(NULL, MY_PORT, &hints, &serverinfo);
	if (getaddrinfores) {
		printf("Error in getaddrinfo: %s\n\n", gai_strerror(getaddrinfores));
		return EXIT_FAILURE;
	}

	// loop through all the results and bind to the first one we can
	int yes = 1;
	for (p = serverinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("Error creating socket: ");
      continue;
    }

    // Fixing the address already in use issue in this code
    if (setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof yes)) {
      perror("setsockopt error: ");
      continue;
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("Error binding a socket: ");
      continue;
    }

    break;
	}

  freeaddrinfo(serverinfo);

  if (p == NULL) {
    printf("led to bind a server port");
  }

	int listenres = listen(sockfd, BACKLOG);
	if (listenres < 0) {
		perror("Error listening for connections: ");
	}

  while(1) {
    socklen_t addr_size = sizeof their_addr;
    int connection_fd = accept(sockfd, (struct sockaddr*)&their_addr, &addr_size);
    if (connection_fd ==  -1) {
      perror("Error accepting connections: ");
      continue;
    }

    int msglen = 0;
    int bytesreceived;
    while (BUFLEN - msglen > 0 && (bytesreceived = recv(connection_fd, buf + msglen, BUFLEN - msglen, 0)) >= 0) {
        msglen += bytesreceived;
        if (msglen > 3 
            && buf[msglen - 1] == '\n' 
            && buf[msglen - 2] == '\n'
            && buf[msglen - 3] == '\n'
            && buf[msglen - 4] == '\n') {
          printf("Received end of message\n\n");
          break;
        }

        if (bytesreceived == 0) {
          printf("Received no data\n\n");
          break;
        }

        printf("Received %d bytes\n\n", bytesreceived);
    }        

    if (bytesreceived < 0) {
      perror("Error receiving data");
    }

    if (bytesreceived > 0 && buf[bytesreceived - 1] != '\0') {
        buf[bytesreceived] = '\0';
        msglen += 1;
    }

    printf("Message: %s\n\n", buf);

    int bytessent = 0;
    int sendres = 0;
    while(bytessent >= 0 && bytessent < bytesreceived) {
      sendres = send(connection_fd, buf + bytessent, msglen - bytessent, 0);
      if (sendres < 0) {
        perror("Error sending message: ");
        return EXIT_FAILURE;
      } else {
        bytessent += sendres;
      }
    }

    // Zero out buffer, variables and close connection
    bytesreceived = 0;
    bytessent = 0;
    memset(buf, 0, BUFLEN);
    close(connection_fd);
  }

	return EXIT_SUCCESS;
}