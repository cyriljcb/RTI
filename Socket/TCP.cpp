#include "TCP.h"
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>// pour memset
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>

int ServerSocket(int port1)
{
		int sEcoute1;
		char ports[10];//faut convertir le int en char
	sprintf(ports, "%d", port1);
	struct addrinfo hints;
	struct addrinfo *results;
	if ((sEcoute1 = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Erreur de socket()");
		exit(1);
	}
	printf("socket creee = %d\n",sEcoute1);
	//setsEcoute(port);
	char host[NI_MAXHOST];	
	char port[NI_MAXSERV];
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
	// pour une connexion passive
	if (getaddrinfo(NULL,ports,&hints,&results) != 0)
		exit(1);
	// Affichage du contenu de l'adresse obtenue

	getnameinfo(results->ai_addr,results->ai_addrlen,host,NI_MAXHOST,port,NI_MAXSERV,NI_NUMERICSERV | NI_NUMERICHOST);
	printf("Mon Adresse IP: %s -- Mon Port: %s\n",host,port);
	printf("\nsEcoute %d\n",sEcoute1);

	if (bind(sEcoute1,results->ai_addr,results->ai_addrlen) < 0)
	{
		perror("Erreur de bind()");
		exit(1);
	}
	freeaddrinfo(results);
	printf("bind() reussi !\n");
	if (listen(sEcoute1,SOMAXCONN) == -1)
	{
		perror("Erreur de listen()");
		exit(1);
	}

	return sEcoute1;
} 

int Accept(int sEcoute, char* ipClient)
{
	int sSocket;
	char host[NI_MAXHOST];
	 char port[NI_MAXSERV];
	
	if((sSocket=accept(sEcoute,NULL,NULL))==-1)
	{
		perror("Erreur de accept()");
	 exit(1);
	} 
	 struct sockaddr_in adrClient;
	 socklen_t adrClientLen = sizeof(struct sockaddr_in);
	 getpeername(sSocket,(struct sockaddr*)&adrClient,&adrClientLen);
	 getnameinfo((struct sockaddr*)&adrClient,adrClientLen,
	 host,NI_MAXHOST,
	 port,NI_MAXSERV,
	 NI_NUMERICSERV | NI_NUMERICHOST);
	 // Copie la valeur de host dans ipClient s'il est non NULL
	if (ipClient != NULL) 
	{
	    strcpy(ipClient, host);
	}
	printf("Mon Adresse IP: %s -- Mon Port: %s\n",host,port);
	 return sSocket;
}

int ClientSocket(char* ipServeur,int portServeur)
{
	int sClient;
	char port[10];//faut convertir le int en char
	sprintf(port, "%d", portServeur);
	 // Creation de la socket
	 if ((sClient = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	 {
	 perror("Erreur de socket()");
	 exit(1);
	 }
	 struct addrinfo hints;
	struct addrinfo *results;
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_NUMERICSERV;
	if (getaddrinfo(ipServeur,port,&hints,&results) != 0)
	exit(1);
	// Demande de connexion
	if (connect(sClient,results->ai_addr,results->ai_addrlen) == -1)
	{
	perror("Erreur de connect()");
	exit(1);
	}
	printf("connect() reussi !\n");
	return sClient;
}

int Send(int sSocket,char* data,int taille)
{
if (taille > TAILLE_MAX_DATA)
return -1;
// Preparation de la charge utile
char trame[TAILLE_MAX_DATA+2];
memcpy(trame,data,taille);
trame[taille] = '#';
trame[taille+1] = ')';
// Ecriture sur la socket
return write(sSocket,trame,taille+2)-2;
}
int Receive(int sSocket,char* data)
{
	bool fini = false;
	int nbLus, i = 0;
	char lu1,lu2;
	while(!fini)
	{
		if ((nbLus = read(sSocket,&lu1,1)) == -1)
			return -1;
		if (nbLus == 0) return i;
		// connexion fermee par client
		if (lu1 == '#')
		{
			if ((nbLus = read(sSocket,&lu2,1)) == -1)
				return -1;
			if (nbLus == 0) return i;
		// connexion fermee par client
			if (lu2 == ')') fini = true;
			else
			{
				data[i] = lu1;
				data[i+1] = lu2;
				i += 2;
			}
		}
		else
		{
			data[i] = lu1;
			i++;
		}
	}
	return i;
}
char GetIP()
{
	// Pour la recherche
	struct addrinfo hints;
	struct addrinfo *results;
	// Pour l'affichage des resultats
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];
	struct addrinfo* info;
	// On fournit l'hote et le service
	memset(&hints,0,sizeof(struct addrinfo)); // initialisation à 0
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo("moon","50000",&hints,&results) != 0)
	printf("Erreur de getaddrinfo");
	else
	{
		// Affichage du contendu des adresses obtenues au format numérique
		for (info = results ; info != NULL ; info = info->ai_next)
		{
			getnameinfo(info->ai_addr,info->ai_addrlen,
			host,NI_MAXHOST,
			port,NI_MAXSERV,
			NI_NUMERICSERV | NI_NUMERICHOST);
			printf("Adresse IP: %s -- Port: %s\n",host,port);
			
		}
	}
	return *host;
}
