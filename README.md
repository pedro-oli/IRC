# IRC
Projeto de SSC0142 - Redes de Computadores

Compilado com g++ versão 5.4.0 (Ubuntu 5.4.0)

## Grupo
* 10408390 Danilo Leonssio Alves
* 8922006 Pedro Henrique Siqueira de Oliveira

## Como rodar
Abra 2 terminais:

* No primeiro, encontre a pasta "servidor" e digite:
```
make all
./servidor <Porta>
```
> Ex.: ./servidor 1234

* No segundo, encontre a pasta "cliente" e digite:
```
make all
./cliente localhost <Porta> [<Apelido>]
```
> Ex.: ./cliente localhost 1234
