#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "limits.h"

#define BACKLOG 10

enum {
    PROGNAME,
    PORT,
    NARGS
};

int main(int argc, char *argv[])
{
    if (argc < NARGS) {
	std::cerr << "Usage: " << argv[PROGNAME]
		  << " <port>" << std::endl;
	exit(1);
    }

    int status;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(0, argv[PORT], &hints, &res)) != 0) {
	std::cerr << "getaddrinfo error: " << gai_strerror(status)
		  << std::endl;
	exit(1);
    }

    // create socket to listen
    int sockfd;
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    listen(sockfd, BACKLOG);

    // accept connection
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    int new_fd;

    addr_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

    char buf[MSG_LEN_MAX];
    std::string msg;
    int bytes_recv, bytes_send;

    // receive nick
    std::string nick;
    recv(new_fd, buf, MSG_LEN_MAX, 0);
    nick = buf;

    do {

	// receive msg
	bytes_recv = recv(new_fd, buf, MSG_LEN_MAX, 0);

	// echo
	bytes_send = send(new_fd, buf, MSG_LEN_MAX, 0);

	msg = buf;

	// add nick in front of msg
	msg.insert(0, ": ");
	msg.insert(0, nick);

	if (bytes_recv != 0) {
	    //std::cout << bytes_recv << std::endl;
	    std::cout << msg << std::endl;
	}

    } while(bytes_recv != 0);

    freeaddrinfo(res);
}
