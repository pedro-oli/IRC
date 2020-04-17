#ifndef _CLIENTE_H
#define _CLIENTE_H

#include <string>
#include <vector>
#include <list>
#include "Socket.h"

class Cliente;

class Cliente
{
public:
    Cliente() : _debug(false) {};

    bool InitSocket();
    bool Connect(char* /*host*/, int /*port*/);
    void Disconnect();
    bool Connected() { return _socket.Connected(); };
    bool SendIRC(std::string /*data*/);
    void ReceiveData();
    void SetNickname(std::string nick) { _nick = nick; };
    void Debug(bool debug) { _debug = debug; };

private:
    Socket _socket;

    std::string _nick;

    bool _debug;
};

#endif