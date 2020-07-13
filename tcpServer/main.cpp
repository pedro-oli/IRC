#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <sstream>

#include <bits/stdc++.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <Windows.h> // Debug: sleep

#include "Cliente.h"
#define MAXCLIENTES 100

using namespace std;

fd_set master;
SOCKET listening;
Cliente clientes[MAXCLIENTES];
vector<string> canais;
int numero_clientes = 0;
bool ctrlC = false;

bool isNicknameValid(string str)
{
    if (str.size() > 50)
        return false;

    for (auto c : str) {
        if (static_cast<unsigned char>(c) > 127) {
            return false;
        }
    }

    return true;
}

string getClienteCanal(SOCKET* sock)
{
    for (int i = 0; i < MAXCLIENTES; i++)
    {
        if (clientes[i].GetId() == (int)*sock)
            return clientes[i].GetCanal();
    }
    return "";
}

string getClienteNickname(SOCKET* sock)
{
    for (int i = 0; i < MAXCLIENTES; i++)
    {
        if (clientes[i].GetId() == (int)*sock)
            return clientes[i].GetNickname();
    }
    return "";
}

void novoCliente(SOCKET* sock, string nickname)
{
    struct sockaddr_in sa;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    getpeername(*sock, (struct sockaddr *)&sa, &addr_size);
    char *newIp = new char[20];
    strcpy(newIp, inet_ntoa(sa.sin_addr));
    string strIp(newIp);

    int i;
    for (i = 0; i < MAXCLIENTES; i++)
        if (clientes[i].GetId() == -1)
            break;

    if (i < MAXCLIENTES)
    {
        clientes[i]._Canal = "";
        clientes[i]._Id = (int)*sock;
        clientes[i]._Ip = strIp;
        clientes[i]._Nickname = nickname;
        // clientes[i].Print(); // Debug
        numero_clientes++;
    }
}

void removeCliente(SOCKET sock)
{
    FD_CLR(sock, &master);
    closesocket(sock);
    for (int i = 0; i < MAXCLIENTES; i++)
    {
        if (clientes[i].GetId() == (int)sock)
        {
            clientes[i]._Id = -1;
            clientes[i]._Nickname = "";
        }
    }

    numero_clientes--;
}

bool canalValido(string canal)
{
    // Verificar se comeca com #
    if (canal.front() != '#')
        return false;

    // Caracteres invalidos:
    size_t found;
    found = canal.find(' ');
    if (found != string::npos)
        return false;
    found = canal.find(',');
    if (found != string::npos)
        return false;

    // Nome valido
    return true;
}

void parseComando(SOCKET sock, string cmd)
{
    // Pegar indice do cliente que enviou o comando
    int indexAtual = -1;
    for (int i = 0; i < MAXCLIENTES; i++)
    {
        if (clientes[i].GetId() == (int)sock)
            indexAtual = i;
    }

    // Novo cliente!
    if (indexAtual == -1)
    {
        // Remover "/new " do comeco (ver main.cpp do projeto de Cliente)
        string nickname = cmd.substr(5);

        // Verificar ASCII e tamanho < 50
        if (isNicknameValid(nickname))
        {
            for (int i = 0; i < MAXCLIENTES; i++)
            {
                // Nickname ja existe
                if (clientes[i].GetNickname() == nickname && clientes[i].GetId() != -1)
                {
                    ostringstream jaExisteOSS;
                    jaExisteOSS << "Servidor: Erro! Este nickname ja esta sendo usado, tente novamente:" << endl;
                    string jaExisteMsg = jaExisteOSS.str();
                    send(sock, jaExisteMsg.c_str(), jaExisteMsg.size() + 1, 0);
                    return;
                }
            }
            // Criar cliente
            novoCliente(&sock, nickname);
            ostringstream successOSS;
            successOSS << "Servidor: Sucesso!";
            string successMsg = successOSS.str();
            send(sock, successMsg.c_str(), successMsg.size(), 0);
            return;
        }
        // Contem caracteres nao ASCII
        // OU tem mais de 50 caracteres
        else
        {
            ostringstream invalidoOSS;
            invalidoOSS << "Servidor: Erro! Seu nickname deve conter no maximo 50 caracteres ASCII, tente novamente: " << endl;
            string invalidoMsg = invalidoOSS.str();
            send(sock, invalidoMsg.c_str(), invalidoMsg.size() + 1, 0);
        }
    }

    // Quit
    else if (cmd == "/quit")
        removeCliente(sock);

    // Ping
    else if (cmd == "/ping")
        send(sock, "pong", 5, 0);

    // Join
    else if (cmd.substr(0, 5) == "/join")
    {
        string canal = cmd.substr(6);
        cout << "Debug: Socket #" << sock << " enviou /join " << canal << endl; // debug

        // Valida o nome do canal
        if (!canalValido(canal))
        {
            ostringstream canalInvalidoOSS;
            canalInvalidoOSS << endl << "Servidor: Nome de canal invalido!" << endl;
            canalInvalidoOSS << "O nome deve comecar com #, nao conter espacos ou virgulas." << endl;
            string canalInvalidoMsg = canalInvalidoOSS.str();
            send(sock, canalInvalidoMsg.c_str(), canalInvalidoMsg.size() + 1, 0);
        }
        else
        {
            // Verificar se ja esta no canal
            if (clientes[indexAtual]._Canal == canal)
            {
                ostringstream jaEstaNoCanalOSS;
                jaEstaNoCanalOSS << endl << "Servidor: Voce ja esta neste canal!";
                string jaEstaNoCanalMsg = jaEstaNoCanalOSS.str();
                send(sock, jaEstaNoCanalMsg.c_str(), jaEstaNoCanalMsg.size() + 1, 0);
                return;
            }

            // Setar a string canal e desmutar o cliente
            clientes[indexAtual]._Canal = canal;
            clientes[indexAtual]._Mute = false;

            // Entrar no canal, se ele ja existe
            if (find(canais.begin(), canais.end(), canal) != canais.end())
            {
                clientes[indexAtual]._Admin = false;
                ostringstream entrouCanalOSS;
                entrouCanalOSS << endl << "Servidor: Voce entrou no canal " << canal << endl;
                string entrouCanalMsg = entrouCanalOSS.str();
                send(sock, entrouCanalMsg.c_str(), entrouCanalMsg.size(), 0);
            }
            // Criar canal
            else
            {
                clientes[indexAtual]._Admin = true;
                canais.push_back(canal);
                ostringstream criouCanalOSS;
                criouCanalOSS << endl << "Servidor: Voce criou o canal " << canal << endl;
                string criouCanalMsg = criouCanalOSS.str();
                send(sock, criouCanalMsg.c_str(), criouCanalMsg.size(), 0);
            }
        }
    }
    // Nickname
    else if (cmd.substr(0, 9) == "/nickname")
    {
        string newNickname = cmd.substr(10);
        cout << "Debug: Socket #" << sock << " enviou /nickname " << newNickname << endl; // Debug
        cout << "Debug: Mudou nome de " << clientes[indexAtual].GetNickname() << " para " << newNickname << endl; // Debug
        clientes[indexAtual]._Nickname = newNickname;
        return;
    }

    // Abaixo estao os comandos exclusivos de administrador:
    else if(clientes[indexAtual].GetAdmin())
    {
        // Kick
        if (cmd.substr(0, 5) == "/kick")
        {
            string nick = cmd.substr(6);
            for (int i = 0; i < MAXCLIENTES; i++)
            {
                if (clientes[i].GetNickname() == nick)
                {
                    // Loop de todos os sockets
                    for (int i = 0; i < (signed int)master.fd_count; i++)
                    {
                        SOCKET removerSock = master.fd_array[i];
                        if (getClienteNickname(&removerSock) == nick)
                        {
                            // Enviar mensagem para cliente kickado
                            ostringstream kickOSS;
                            kickOSS << endl << "Servidor: Voce foi kickado";
                            string kickMsg = kickOSS.str();
                            send(removerSock, kickMsg.c_str(), kickMsg.size() + 1, 0);
                            removeCliente(removerSock);
                        }
                    }

                    // Encontrou
                    return;
                }
            }

            // Nao encontrou ninguem
            ostringstream whoisFailOSS;
            whoisFailOSS << endl << "Servidor: Nao existe ninguem chamado " << nick;
            string whoisFailMsg = whoisFailOSS.str();
            send(sock, whoisFailMsg.c_str(), whoisFailMsg.size() + 1, 0);
            return;
        }
        // Mute
        else if (cmd.substr(0, 5) == "/mute")
        {
            string nick = cmd.substr(6);
            for (int i = 0; i < MAXCLIENTES; i++)
            {
                if (clientes[i].GetNickname() == nick)
                {
                    clientes[i]._Mute = true;
                    // Avisar administrador
                    ostringstream muteSuccessOSS;
                    muteSuccessOSS << endl << "Servidor: Voce mutou " << nick;
                    string muteSuccessMsg = muteSuccessOSS.str();
                    send(sock, muteSuccessMsg.c_str(), muteSuccessMsg.size() + 1, 0);
                }
            }
            return;
        }
        // Unmute
        else if (cmd.substr(0, 7) == "/unmute")
        {
            string nick = cmd.substr(8);
            for (int i = 0; i < MAXCLIENTES; i++)
            {
                if (clientes[i].GetNickname() == nick)
                {
                    clientes[i]._Mute = false;
                    // Avisar administrador
                    ostringstream unmuteSuccessOSS;
                    unmuteSuccessOSS << endl << "Servidor: Voce desmutou " << nick;
                    string unmuteSuccessMsg = unmuteSuccessOSS.str();
                    send(sock, unmuteSuccessMsg.c_str(), unmuteSuccessMsg.size() + 1, 0);
                }
            }
            return;
        }
        // Who is
        else if (cmd.substr(0, 6) == "/whois")
        {
            string nick = cmd.substr(7);
            for (int i = 0; i < MAXCLIENTES; i++)
            {
                if (clientes[i].GetNickname() == nick)
                {
                    ostringstream whoisOSS;
                    whoisOSS << endl << "Servidor: O IP de " << nick << ": " << clientes[i].GetIp();
                    string whoisMsg = whoisOSS.str();
                    send(sock, whoisMsg.c_str(), whoisMsg.size() + 1, 0);
                    return;
                }
            }

            // Nao encontrou ninguem
            ostringstream whoisFailOSS;
            whoisFailOSS << endl << "Servidor: Nao existe ninguem chamado " << nick;
            string whoisFailMsg = whoisFailOSS.str();
            send(sock, whoisFailMsg.c_str(), whoisFailMsg.size() + 1, 0);
        }
    }
}

void sendToChannel(string canalAtual, SOCKET sock, string buf)
{
    // Verificar se cliente esta mutado
    for (int i = 0; i < MAXCLIENTES; i++)
    {
        if (clientes[i].GetId() == (int)sock && clientes[i].GetMute())
        {
            ostringstream muteOSS;
            muteOSS << endl << "Servidor: Voce foi mutado pelo administrador do canal " << canalAtual << endl;
            string muteMsg = muteOSS.str();
            send(sock, muteMsg.c_str(), muteMsg.size(), 0);
            return;
        }
    }

    for (int i = 0; i < (signed int)master.fd_count; i++)
    {
        SOCKET outSock = master.fd_array[i];

        // Ignorar o socket de listening
        if (outSock != listening)
        {
            string sender = getClienteNickname(&sock);
            ostringstream ss;

            // Echo
            if (outSock == sock)
                ss << sender << ": " << buf << endl;
            // Broadcast
            else if (getClienteCanal(&outSock) == canalAtual)
                ss << endl << sender << ": " << buf << endl;

            string strOut = ss.str();
            send(outSock, strOut.c_str(), strOut.size(), 0);
        }
    }
}

void finalizar()
{
    // Remover o socket listening de master, impedindo que alguem tente conectar
	FD_CLR(listening, &master);
	closesocket(listening);

	// Avisar clientes
	string terminateMsg = "Servidor: O servidor esta desligando...";
	while (master.fd_count > 0)
	{
		SOCKET sock = master.fd_array[0];
        send(sock, terminateMsg.c_str(), terminateMsg.size() + 1, 0);

        // Remover do master e fechar o socket atual:
        FD_CLR(sock, &master);
        closesocket(sock);
	}
	ctrlC=true;
}

void handleCtrlC(int s)
{
    cout << "Voce derrubou o servidor! Desligando..." << endl; // Debug
    finalizar();
}

int main(int argc, char** argv)
{
    // Ignorar Ctrl-C
    signal(SIGINT, handleCtrlC);

	// Inicializar winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Erro inicializando winsock! Fechando..." << endl;
		return -1;
	}

	// Criar o socket de listening
	listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Erro criando socket de listening! Fechando..." << endl;
		return -2;
	}

	// Bind do endereco e da porta no socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Escutar
	listen(listening, SOMAXCONN);
	// Zerar file descriptor set
	FD_ZERO(&master);
	// Adicionar o socket de listening no file descriptor set
	FD_SET(listening, &master);

	// Loop do select
	while (!ctrlC)
	{
	    // Criar copia para ser consumida no select
		fd_set copy = master;
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Loop de todas conexoes atuais / potencial nova conexao
		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];

			// Nova conexao de cliente:
			if (sock == listening)
			{
			    // Aceitar nova conexao
				SOCKET client = accept(listening, nullptr, nullptr);

				// Adicionar a conexao a lista de clientes conectados
				FD_SET(client, &master);

				// Enviar primeira mensagem ao novo cliente
				ostringstream welcomeOSS;
                welcomeOSS << "Servidor: Bem vindo ao chat! Insira seu nickname:";
                string welcomeMsg = welcomeOSS.str();
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
			}
			// Mensagem de cliente ja existente:
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 1)
                    removeCliente(sock);
				else
				{
				    cout << "Debug: Cliente #" << sock << ": " << buf << endl; // Debug

					// Verificar se cliente mandou um comando
					if (buf[0] == '/')
					{
						string cmd = string(buf, bytesIn - 1);
						parseComando(sock, cmd);
                        continue;
                    }

					// Verificar se cliente ja esta em um canal
					string canalAtual = getClienteCanal(&sock);
					if (canalAtual != "")
                        sendToChannel(canalAtual, sock, (string)buf);
                    // Nao entrou em nenhum canal ainda
                    else
                    {
                        ostringstream semCanalOSS;
						semCanalOSS << endl << "Servidor: Voce nao esta em nenhum canal! ";
						semCanalOSS << "Digite '/join <nomeCanal>' para entrar em algum.";
						string semCanalMsg = semCanalOSS.str();
                        send(sock, semCanalMsg.c_str(), semCanalMsg.size() + 1, 0);
                    }
				}
			}
		}
	}

    // Limpar winsock
	WSACleanup();
	return 0;
}
