#include "SMOP.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//***** Etat du protocole : liste des clients loggés ****************
int clients[NB_MAX_CLIENTS];
int nbClients = 0;

//pthread_mutex_t mutexClients = PTHREAD_MUTEX_INITIALIZER;

//***** Parsing de la requete et creation de la reponse *************
bool SMOP(char* requete, char* reponse,int socket)
{
	/*// ***** Récupération nom de la requete *****************
	char *ptr = strtok(requete,",");
	// ***** LOGIN ******************************************
	if (strcmp(ptr,"LOGIN") == 0)
	{
		strtok(NULL,",");
		strtok(NULL,",");
		char user[50], password[50];
		strcpy(user,strtok(NULL,","));
		strcpy(password,strtok(NULL,","));
		printf("\t[THREAD %p] LOGIN de %s\n",pthread_self(),user);
		if (estPresent(socket) >= 0) // client déjà loggé
		{
			sprintf(reponse,"LOGIN#ko#Client déjà loggé !");
			return false;
		}
		else
		{
			if (SMOP_Login(user,password))
			{
				sprintf(reponse,"LOGIN#ok");
				ajoute(socket);
			}
			else
			{
				sprintf(reponse,"LOGIN#ko#Mauvais identifiants !");
				return false;
			}
		}*/
	}
// ***** LOGOUT *****************************************
	/*if (strcmp(ptr,"LOGOUT") == 0)
	{
		printf("\t[THREAD %p] LOGOUT\n",pthread_self());
		retire(socket);
		sprintf(reponse,"LOGOUT#ok");
		return false;
	}
// ***** OPER *******************************************
	if (strcmp(ptr,"OPER") == 0)
	{
		char op;
		int a,b;
		ptr = strtok(NULL,"#");
		op = ptr[0];
		a = atoi(strtok(NULL,"#"));
		b = atoi(strtok(NULL,"#"));
		printf("\t[THREAD %p] OPERATION %d %c %d\n",pthread_self(),a,op,b);
		if (estPresent(socket) == -1) sprintf(reponse,"OPER#ko#Client non loggé!");
		else
		{
			try
			{
			int resultat = SMOP_Operation(op,a,b);
			sprintf(reponse,"OPER#ok#%d",resultat);
			}
			catch(int) { sprintf(reponse,"OPER#ko#Division par zéro !"); }
		}
	}
	return true;
}*/
//***** Traitement des requetes *************************************
bool SMOP_Login(const char* user,const char* password)
{
	if (strcmp(user,"wagner")==0 && strcmp(password,"abc123")==0) return true;
	if (strcmp(user,"charlet")==0 && strcmp(password,"xyz456")==0) return true;
	return false;
}

int SMOP_Operation(char op,int a,int b)
{
	if (op == '+') return a+b;
	if (op == '-') return a-b;
	if (op == '*') return a*b;
	if (op == '/')
	{
		if (b == 0) throw 1;
		return a/b;
	}
	return 0;
}
//***** Gestion de l'état du protocole ******************************
/*int estPresent(int socket)
{
	int indice = -1;
	pthread_mutex_lock(&mutexClients);
	for(int i=0 ; i<nbClients ; i++)
	if (clients[i] == socket) { indice = i; break; }
	pthread_mutex_unlock(&mutexClients);
	return indice;
}
void ajoute(int socket)
{
	pthread_mutex_lock(&mutexClients);
	clients[nbClients] = socket;
	nbClients++;
	pthread_mutex_unlock(&mutexClients);
}
void retire(int socket)
{
	int pos = estPresent(socket);
	if (pos == -1) return;
	pthread_mutex_lock(&mutexClients);
	for (int i=pos ; i<=nbClients-2 ; i++)
	clients[i] = clients[i+1];
	nbClients--;
	pthread_mutex_unlock(&mutexClients);
}*/
//***** Fin prématurée **********************************************
void SMOP_Close()
{
	pthread_mutex_lock(&mutexClients);
	for (int i=0 ; i<nbClients ; i++)
	close(clients[i]);
	pthread_mutex_unlock(&mutexClients);
}