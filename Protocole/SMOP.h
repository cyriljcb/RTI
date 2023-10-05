#ifndef SMOP_H
#define SMOP_H
#define NB_MAX_CLIENTS 100

bool SMOP(char* requete, char* reponse,int socket);
bool SMOP_Login(const char* user,const char* password);
int SMOP_Operation(char op,int a,int b);
void SMOP_Close();




#endif