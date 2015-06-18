#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "hacking.h"

#define PORT 7890

int main(void)
{
  int sockfd, new_sockfd; // listen on sockfd, new conns on new_
  struct sockaddr_in host_addr, client_addr;
  socklen_t sin_size;
  int recv_length=1, yes=1;
  char buffer[1024];


  printf("Creating socket...\n");
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    fatal("in socket");
  printf("Setting socket options...\n");
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    fatal("Setting socket option SO_REUSEADDR");

  host_addr.sin_family = AF_INET; // host byte order
  host_addr.sin_port = htons(PORT);  // short network order
  host_addr.sin_addr.s_addr = 0; // auto fill with my ip
  memset(&(host_addr.sin_zero), '\0', 8);  // zero the rest

  printf("Binding Socket...\n");
  if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1)
    fatal("Binding to socket");

  printf("Listening on Socket...\n");
  if (listen(sockfd, 5) == -1)
    fatal("Listening on socket");

  printf("Boom! Off to the races! Hit me up, brah - port %d...\n", PORT);
  while(1) { //accept loop
    sin_size = sizeof(struct sockaddr_in);
    new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
    if(new_sockfd == -1)
      fatal("accepting connections");
    printf("Server got connection frm %s port %d\n",
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    send(new_sockfd, "HUllo\n", 6, 0);
    recv_length = recv(new_sockfd, &buffer, 1024, 0);
    while(recv_length > 0) {
      printf("RECV: %d bytes\n", recv_length);
      dump(buffer, recv_length);
      recv_length = recv(new_sockfd, &buffer, 1024, 0);
    }
    close(new_sockfd);
  } // end while
  return 0;
}
