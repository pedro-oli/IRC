# IRC
Projeto de SSC0142 - Redes de Computadores

Compilado com mingw32-g++ (Windows 10), como descrito a seguir:
> mingw32-g++.exe -g -std=c++11 -c <diretório_raiz>\tcpClient\main.cpp -o obj\Debug\main.o

> mingw32-g++.exe -o <diretório_raiz>\tcpClient\bin\Debug\tcpClient.exe obj\Debug\main.o -lws2_32

## Grupo
* 8922006 Pedro Henrique Siqueira de Oliveira

## Como rodar
1. Compile os 2 projetos (serão gerados 2 executáveis)
2. Execute o tcpServer.exe;
3. Altere o IP em "tcpCliente.bat" para o IP desejado (da máquina onde foi executado o tcpServer.exe)
4. Execute o tcpClient.bat (quantas vezes desejar).