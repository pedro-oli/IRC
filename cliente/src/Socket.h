#ifndef _SOCKET_H
#define _SOCKET_H

#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket(s) close(s)
#define close(s)
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

class Socket
{
public:
    bool Init();
    bool Connect(char const* host, int port);
    void Disconnect();
    bool Connected() { return _connected; };
    bool SendData(char const* data);
    std::string ReceiveData();

private:
    int _socket;
    bool _connected;
};

#endif