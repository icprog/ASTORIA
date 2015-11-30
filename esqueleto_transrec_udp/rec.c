/* Copyright (C) 2006 PRAV - Pesquisa em Redes de Alta Velocidade
 *                    NTVD - Núcleo de TV Digital
 * http://www.ufrgs.br/ntvd
 *
 *  O objetivo deste programa é apresentar a base da estrutura de programação com sockets
 *  através de UDP
 *
 * Serv.c: Esqueleto de servidor UDP.
 * Argumentos: -p <porta>
 *
 * Desenvolvido para sistemas UNIX Like (Linux, FreeBSD, NetBSD...) e Windows
 *		Maiko de Andrade
 *		Valter Roesler
*/

#include <stdio.h>
#include <string.h>

#define SOCKET int
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main(int argc, char **argv){
	 struct  sockaddr_in peer;
	 SOCKET  s;
	 int porta, peerlen, rc, i;
	 char buffer[100];

// Cria o socket na familia AF_INET (Internet) e do tipo UDP (SOCK_DGRAM)
	 if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		  printf("Falha na criacao do socket\n");
		  exit(1);
 	 }

// Define domínio, IP e porta a receber dados
	 memset((void *) &peer,0,sizeof(struct sockaddr_in));
	 peer.sin_family = AF_INET;
	 peer.sin_addr.s_addr = htonl(INADDR_ANY); // Recebe de qualquer IP
	 peer.sin_port = htons(2023); // Recebe na porta especificada na linha de comando
	 peerlen = sizeof(peer);

// Associa socket com estrutura peer
	 if(bind(s,(struct sockaddr *) &peer, peerlen)) {
		  printf("Erro no bind\n");
		  exit(1);
	 }

	 printf("Socket inicializado. Aguardando mensagens...\n\n");

// Recebe pacotes do cliente e responde com string "ACK"
	 while (1)
	 {
		 // Quando recebe um pacote, automaticamente atualiza o IP da estrutura peer
		 rc = recvfrom(s,buffer,sizeof(buffer),0,(struct sockaddr *) &peer,(socklen_t *)&peerlen);
		 printf("Recebido %s\n", &buffer);
	 }

}
