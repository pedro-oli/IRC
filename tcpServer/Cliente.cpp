#include "Cliente.h"

Cliente::Cliente()
    : _Admin(false), _Canal(""), _Id(-1), _Ip(""), _Nickname(""), _Mute(false)
{
}

void Cliente::Print()
{
    cout << "Debug: Id=" << _Id << " Canal=" << _Canal << " Admin=" << _Admin;
    cout << " Ip=" << _Ip << " Nickname=" << _Nickname << " _Mute=" << _Mute << endl;
}

Cliente::~Cliente()
{
    // destructor
}
