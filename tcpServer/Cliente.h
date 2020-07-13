#ifndef CLIENTE_H
#define CLIENTE_H

#include <iostream>
#include <string>
#include <Winsock2.h>
#include <WS2tcpip.h>

using namespace std;

class Cliente
{
    public:
        bool _Admin;
        string _Canal;
        int _Id;
        string _Ip;
        string _Nickname;
        bool _Mute;

        Cliente();
        virtual ~Cliente();

        bool GetAdmin() { return _Admin; }
        string GetCanal() { return _Canal; }
        int GetId() { return _Id; }
        string GetIp() { return _Ip; }
        string GetNickname() { return _Nickname; }
        bool GetMute() { return _Mute; }

        void Print(); // Debug
};

#endif // CLIENTE_H
