#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "limits.h"

enum {
    PROGNAME,
    ADDRESS,
    PORT,
    NICK,
    NARGS
};

int main(int argc, char *argv[])
{
    if (argc < NARGS) {
	std::cerr << "Usage: " << argv[PROGNAME]
		  << "<address> <port> <nick>" << std::endl;
	exit(1);
    }

    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(argv[ADDRESS], argv[PORT], &hints, &servinfo)) != 0) {
	std::cerr << "getaddrinfo error: " << gai_strerror(status)
		  << std::endl;
	exit(1);
    }

    int sockfd;
    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,
		    servinfo->ai_protocol);

    // connect with the server
    connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);

    // send nick information
    send(sockfd, argv[NICK], strlen(argv[NICK]), 0);

    int count, bytes_send, bytes_recv;
    char buf[MSG_LEN_MAX];
    std::string input, chunk, answer;

    do {

	std::getline(std::cin, input);
	//std::cout << input.length() << std::endl;
	count = 0;

	do {

	    // send msg and break it if necessary
	    chunk = input.substr(count, MSG_LEN_MAX);
	    bytes_send = send(sockfd, chunk.data(), MSG_LEN_MAX, 0);
	    //std::cout << count << " " << chunk << std::endl;
	    count += bytes_send;

	    // receive echo
	    bytes_recv = recv(sockfd, buf, MSG_LEN_MAX, 0);
	    answer = buf;
	    answer.insert(0, "server: ");
	    std::cout << answer << std::endl;

	} while (count < input.length());

    } while(input.compare("quit") != 0);

    freeaddrinfo(servinfo);
}
