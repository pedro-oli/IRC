#include <iostream>
#include <signal.h>
#include <cstdlib>
#include <map>
#include <algorithm>
#include "Cliente.h"
#include "Thread.h"

ThreadReturn inputThread(void* cliente)
{
    std::string command;
    while(true)
    {
        getline(std::cin, command);
        if (command == "")
            continue;

        ((Cliente*)cliente)->SendIRC(command);
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "Parametros esperados: host porta [nick]" << std::endl;
        return 1;
    }

    char* host = argv[1];
    int port = atoi(argv[2]);
    std::string nick("nickname_padrao");

    if (argc >= 4)
        nick = argv[3];

    std::cout << "Host: " << host;
    std::cout << " Port: " << port;
    std::cout << " Nickname: " << nick << std::endl;

    Cliente cliente;
    cliente.SetNickname(nick);
    cliente.Debug(true);

    // Thread de input
    Thread thread;
    thread.Start(&inputThread, &cliente);

    if (cliente.InitSocket())
    {
        std::cout << "Socket inicializado. Conectando..." << std::endl;

        if (cliente.Connect(host, port))
        {
            std::cout << "Conectado." << std::endl;
            while (cliente.Connected())
                cliente.ReceiveData();

            if (cliente.Connected())
                cliente.Disconnect();
            std::cout << "Disconectado." << std::endl;
        }
    }
}