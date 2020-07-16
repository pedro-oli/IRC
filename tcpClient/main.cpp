#include <iostream>
#include <sstream>
#include <string>

#include <process.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <Winsock2.h>
#include <WS2tcpip.h>

using namespace std;

SOCKET sock;
bool ctrlC = false;
bool isNicknameValid = false;
bool servidorRespondeu;

// Funcao auxiliar Windows
int inet_pton(int af, const char *src, void *dst)
{
    struct sockaddr_storage ss;
    int size = sizeof(ss);
    char src_copy[INET6_ADDRSTRLEN+1];

    ZeroMemory(&ss, sizeof(ss));
    strncpy (src_copy, src, INET6_ADDRSTRLEN+1);
    src_copy[INET6_ADDRSTRLEN] = 0;

    if (WSAStringToAddress(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0) {
        switch(af) {
            case AF_INET:
                *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
                return 1;
            case AF_INET6:
                *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
                return 1;
        }
    }
    return 0;
}

// Receber mensagens do servidor
void receive_messages(void* sk)
{
    char buf[4096];
    do {
        memset(buf, 0, 4096);
        int bytesReceived = recv((SOCKET)sk, buf, 4096, 0);
        if (bytesReceived == 0)
            break;
        else if (bytesReceived != -1)
        {
            string serverMsg = string(buf, bytesReceived);
            // Verificar se é mensagem de erro
            if (serverMsg.substr(0, 15) == "Servidor: Erro!")
            {
                cout << serverMsg;
                servidorRespondeu = true;
            }
            // Verificar se é mensagem de sucesso
            else if (serverMsg.substr(0, 18) == "Servidor: Sucesso!")
            {
                isNicknameValid = true;
                servidorRespondeu = true;
                cout << "Servidor: Voce se conectou ao servidor!" << endl;
                cout << "Digite '/join <nomeCanal>' para entrar em um canal";
                cout << " (Caso ele nao exista, sera criado)." << endl;
            }
            // Imprimir
            else
                cout << serverMsg;
        }
    } while(!ctrlC);

    // Fechar socket
    // closesocket((SOCKET)sk);
    // WSACleanup();
    if (!ctrlC)
    {
        string aux;
        cout << "Aperte Enter para fechar o programa." << endl;
        getline(cin, aux);
    }
    exit(0);
}

void handleCtrlC(int s){
    // string auxT = "/quit";
    // int sendRes = send(sock, auxT.c_str(), auxT.size() + 1, 0);
    // getline(cin, auxT);
    string auxT2;
    getline(cin, auxT2); // teste
    ctrlC = true;
    // closesocket(sock);
    // exit(0);
}

int main(int argc, char** argv)
{
    // Ignorar Ctrl-C
    signal(SIGINT, handleCtrlC);

    // Inicializar Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "WSAStartup failed." << endl;
    }

    // Criar socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if ((int)sock == -1)
    {
        return -1;
    }

    // Criar estrutura pra conectar no servidor
    int port = 54000;
    // string ipAddress = argv[1]; // IP, pela linha de comando (ver tcpCliente.bat)
    string ipAddress = "127.0.0.1"; // localhost
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    // Conectar no servidor
    int connectRes;
    connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        cout << "Erro ao tentar conectar ao servidor! Desligando..." << endl;
        return -2;
    }

    // Thread do recv (funcao acima do main)
    _beginthread(receive_messages, 0, (void*)sock);

    // Apos mensagem de boas vindas do servidor, ler nickname ate ele ser valido:
    while (!isNicknameValid)
    {
        // Ler novo nickname
        string nickname;
        getline(cin, nickname);
        ostringstream novoNicknameOSS;
        novoNicknameOSS << "/new " << nickname;
        string novoNicknameMsg = novoNicknameOSS.str();

        // Resetar flag e enviar
        servidorRespondeu = false;
        send(sock, novoNicknameMsg.c_str(), novoNicknameMsg.size() + 1, 0);

        // Esperar servidor aprovar:
        while (!servidorRespondeu) {}
    }

    // Loop do send:
    string userInput;
    do {
        // Ler input
        getline(cin, userInput);

        if (userInput.size() != 0)
        {
            // Enviar ao servidor
            int sendRes = send(sock, userInput.c_str(), userInput.size() + 1, 0);
            if (sendRes == -1 || userInput == "/quit")
                break;
        }
    } while(!ctrlC);

    // Fechar socket
    closesocket(sock);
    WSACleanup();
    if (ctrlC)
    {
        cout << "Voce saiu do servidor. Aperte Enter para fechar o programa." << endl;
        getline(cin, userInput);
    }
    return 0;
}
