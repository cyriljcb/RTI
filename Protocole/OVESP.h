#ifndef OVESP_H
#define OVESP_H

#define NB_MAX_CLIENTS 100
#define NUMARTICLEMAX 21
#define TAILLE_MAX 500
//#define IPSERVEUR    "192.168.47.128"
//#define IPSERVEUR    "192.168.126.128"
#define IPSERVEUR    "0.0.0.0"

void AccesBD_OVESP(char* requete, char * reponse,int socket,char* lArticle);

#endif
