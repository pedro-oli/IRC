#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define closesocket(s) close(s)
#define close(s)
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Parametro esperado: port" << std::endl;
        return 1;
    }
    int port = atoi(argv[1]);
    std::cout << "Port: " << port << std::endl;

    int listening = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listening == INVALID_SOCKET)
    {
        std::cout << "Erro de Socket (listening)." << std::endl;
        return 1;
    }

    hostent* he;
    if (!(he = gethostbyname("localhost")))
    {
        std::cout << "Nao foi possivel encontrar o host." << std::endl;
        return 1;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = *((const in_addr*)he->h_addr);
    addr.sin_addr.s_addr = INADDR_ANY;

    std::cout << "Aguardando a conexao do cliente." << std::endl;
    bind(listening, (sockaddr*)&addr, sizeof(addr));
    listen(listening, SOMAXCONN);

    sockaddr_in cliente;
    int tamanhoCliente = sizeof(cliente);
    int socketCliente = accept(listening, (sockaddr*)&cliente, (socklen_t*)&tamanhoCliente);
    if (socketCliente == INVALID_SOCKET)
    {
        std::cout << "Erro de Socket (cliente)." << std::endl;
        return 1;
    }

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);
    if (getnameinfo((sockaddr*)&cliente, sizeof(cliente), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        std::cout << host << " conectou no service: " << service << std::endl;
    }
    else
    {
        inet_ntop(AF_INET, &cliente.sin_addr, host, NI_MAXHOST);
        std::cout << host << " conectou no service: " << ntohs(cliente.sin_port) << std::endl;
    }
    closesocket(listening);

    char buffer[4096];
    while (true)
    {
        memset(buffer, 0, 4096);
        int bytesRecebidos = recv(socketCliente, buffer, 4096, 0);
        if (bytesRecebidos == SOCKET_ERROR)
        {
            std::cout << "Erro em recv()." << std::endl;
            break;
        }

        if (bytesRecebidos == 0)
        {
            std::cout << "Cliente desconectado." << std::endl;
            break;
        }

        // eco:
        std::cout << buffer;
        send(socketCliente, buffer, bytesRecebidos + 1, 0);
    }

    closesocket(socketCliente);
}