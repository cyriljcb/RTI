#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>// pour memset
#include<sys/types.h>
#include <sys/stat.h>
#include<sys/socket.h>
#include<netdb.h>
#include <signal.h>
#include <fcntl.h>
#include<pthread.h>
#include <mysql.h>
#include "TCP.h"
#include "Protocole/OVESP.h"

#define TAILLE_FILE_ATTENTE 20
pthread_mutex_t mutexSocketsAcceptees;
pthread_cond_t condSocketsAcceptees;
int socketsAcceptees[TAILLE_FILE_ATTENTE];
int indiceEcriture=0, indiceLecture=0;
//const char s[4]="/";
  //char* tok;
void HandlerSIGINT(int s);
void* FctThreadClient(void* p);
void traitement(int sService);
void Recevoir(char* requete,int);
void Envoi(char* requete,int);

int sEcoute;
int main(int argc,char* argv[])
{
int sService;
if (argc != 2)
{
	printf("Erreur...\n");
	printf("USAGE : Serveur portServeur\n");
	exit(1);
}
// Initialisation socketsAcceptees
pthread_mutex_init(&mutexSocketsAcceptees,NULL);
pthread_cond_init(&condSocketsAcceptees,NULL);
for (int i=0 ; i<TAILLE_FILE_ATTENTE ; i++)
socketsAcceptees[i] = -1;

// Armement des signaux
struct sigaction A;
A.sa_flags = 0;
sigemptyset(&A.sa_mask);
A.sa_handler = HandlerSIGINT;

// Creation de la socket d'écoute
if ((sEcoute = ServerSocket(atoi(argv[1]))) == -1)
{
perror("Erreur de ServeurSocket");
exit(1);
}
int NB_THREADS_POOL = -1;
FILE *fichier = fopen("config.txt", "r");
    if (fichier == NULL) {
        perror("Impossible d'ouvrir le fichier de configuration");
        exit(1);
    }

    char ligne[100];
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) 
     {
        if (sscanf(ligne, "NB_THREADS_POOL = %d", &NB_THREADS_POOL) == 1)
        {
          break;
        } 
    }
// Creation du pool de threads
printf("Création du pool de threads.\n");
pthread_t th;
for (int i=0 ; i<NB_THREADS_POOL ; i++)
pthread_create(&th,NULL,FctThreadClient,NULL);

if (sigaction(SIGINT,&A,NULL) == -1)
{
	perror("Erreur de sigaction");
	exit(1);
}

	// Mise en boucle du serveur
	char ipClient[50];
	printf("Demarrage du serveur.\n");
	int i=0;
	while(1)
	{
		printf("Attente d'une connexion...\n");
		if ((sService = Accept(sEcoute,NULL)) == -1)
		{
			perror("Erreur de Accept");
			close(sEcoute);
			exit(1);
		}
		
		printf("Connexion acceptée : IP=%s socket=%d\n",ipClient,sService);
		// Insertion en liste d'attente et réveil d'un thread du pool
		// (Production d'une tâche)
		pthread_mutex_lock(&mutexSocketsAcceptees);
		socketsAcceptees[indiceEcriture] = sService; // !!!
		indiceEcriture++;
		if (indiceEcriture == TAILLE_FILE_ATTENTE) 
			indiceEcriture = 0;
		pthread_mutex_unlock(&mutexSocketsAcceptees);
		pthread_cond_signal(&condSocketsAcceptees);
		printf("signale envoyé");
	}
	close(sService);
}

void* FctThreadClient(void* p)
{
int sService;
while(1)
	{
		printf("\npasse dans le thread\n");
		printf("\n\t[THREAD %p] Attente socket...\n",pthread_self());
		// Attente d'une tâche
		pthread_mutex_lock(&mutexSocketsAcceptees);
		while (indiceEcriture == indiceLecture)
		pthread_cond_wait(&condSocketsAcceptees,&mutexSocketsAcceptees);
		
		sService = socketsAcceptees[indiceLecture];
		printf("\npasse dans le thread1 et sService =%d\n",sService);
		socketsAcceptees[indiceLecture] = -1;
		indiceLecture++;
		if (indiceLecture == TAILLE_FILE_ATTENTE) indiceLecture = 0;
		pthread_mutex_unlock(&mutexSocketsAcceptees);
		traitement(sService);
	}

}
void HandlerSIGINT(int s)
{
	printf("\nArret du serveur.\n");
	close(sEcoute);
	pthread_mutex_lock(&mutexSocketsAcceptees);
	for (int i=0 ; i<TAILLE_FILE_ATTENTE ; i++)
		if (socketsAcceptees[i] != -1) 
			close(socketsAcceptees[i]);
	pthread_mutex_unlock(&mutexSocketsAcceptees);
	exit(0);
}
void traitement(int sService)
{
	char requete[500],reponse[500],lArticle[500];
	lArticle[0]='\0';
	bool test = true;
	while(test)
	{
		requete[0]='\0';
		reponse[0]='\0';

				Recevoir(requete,sService);
        printf("requete : %s\n", requete);
        printf("menu\n");
        AccesBD_OVESP(requete, reponse, sService, lArticle);
        //printf("liste article = %s\n", lArticle);
        printf("apres l'acces BD\n");
        if(reponse[0]!='\0')
        {
        	Envoi(reponse,sService);
        }
        else
        {
        	test = false;
        }

	}
}

void Envoi(char* requete,int sService)
{
	int nbEcrits;
	if ((nbEcrits = Send(sService,requete,strlen(requete))) == -1)
	{
		perror("Erreur de Send");
		close(sService);
	}
}

void Recevoir(char* requete,int sService)
{
	int nbLus;
	if ((nbLus = Receive(sService,requete)) < 0)
	{
		perror("Erreur de Receive");
		close(sService);
	}
}