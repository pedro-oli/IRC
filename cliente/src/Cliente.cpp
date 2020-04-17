#include <iostream>
#include <algorithm>
#include "Socket.h"
#include "Cliente.h"

bool Cliente::InitSocket()
{
    return _socket.Init();
}

bool Cliente::Connect(char* host, int port)
{
    return _socket.Connect(host, port);
}

void Cliente::Disconnect()
{
    _socket.Disconnect();
}

bool Cliente::SendIRC(std::string data)
{
    std::string start = _nick;
    start.append(": ");
    data.insert(0, start);
    data.append("\n");
    return _socket.SendData(data.c_str());
}

void Cliente::ReceiveData()
{
    std::string buffer = _socket.ReceiveData();

    std::string line;
    std::istringstream iss(buffer);
    while(getline(iss, line))
    {
        if (line.find("\r") != std::string::npos)
            line = line.substr(0, line.size() - 1);
        std::cout << line << std::endl;
    }
}