#include"OVESP.h"
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
#include <cstring>
#include <cstdio>



int clients[NB_MAX_CLIENTS];
int nbclients = 0;
MYSQL* connexion;
const char s[4]="/";
const char dollar[2]="$";
char* tok;

int suppArticle(char* lArticle,char* m);
void ajouterArticle(char* lArticle,char* c);
void Connexion_OVESP();
void FctLogin(char* requete, char* reponse, int socket);
void FctConsult(char* requete,char * reponse);
void FctAchat(char* requete,char * reponse,char* lArticle);
void FctCancel(char* requete,char * reponse,char* lArticle);
void FctCaddie(char* requete, char* reponse,char* lArticle);
int estPresent(int socket);
void ajoute(int socket);
void retire(int socket);
int recupererNbrArticle(char* lArticle);
char* remplacerpoint(char *str);
int rechercherQuantite(char *ind,char *str);
pthread_mutex_t mutexClients = PTHREAD_MUTEX_INITIALIZER;

void Connexion_OVESP()
{
	if(connexion==NULL)
	{
		connexion = mysql_init(NULL);
		  if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
		  {
		    printf("(SERVEUR) Erreur de connexion à la base de données...\n");
		    exit(1);  
		  }
	}
		
}

void AccesBD_OVESP(char* requete, char * reponse,int socket,char * lArticle)
{

	char req[500],opt[20];
	strcpy(req,requete);
	printf("requete : %s & req : %s\n",requete,req);
	tok=strtok(req,s);
	printf("tok : %s\n",tok);
	Connexion_OVESP();
	if (strcmp(tok, "LOGIN") == 0)
	{
		printf("login\n");
		FctLogin(requete,reponse,socket);
	}
	if(strcmp(tok,"CONSULT")==0)
	{

		printf("CONSULT\n");
		FctConsult(requete,reponse);
	}
	
	if(strcmp(tok,"ACHAT")==0)
	{
		FctAchat(requete,reponse,lArticle);
		printf("AMMMMMMMMMMMMMM\n");
	}
	if(strcmp(tok,"CANCEL")==0)
	{
		FctCancel(requete,reponse,lArticle);
		printf("sors du case CANCEL\n");
	}
	/*if(strcmp(tok,"CADDIE")==0)
	{
		FctCaddie(requete,reponse,lArticle);
	}*/
	printf("(AccesBD)\n\nrequete : %s\nreponse : %s\nlArticle : %s\n\n\n",requete,reponse,lArticle);
}

void FctLogin(char* requete,char * reponse,int socket) //attention, on va écrire dans reponse créé dans serveur
{
	MYSQL_RES  *resultat;
  	MYSQL_ROW  Tuple;
	printf("passage dans le case LOGIN de AccesBD\n");
		char nom[20],mdp[20],message[50],communication[200],sta[5],requeteSQL[200];
		int nouveau = -1,status;
		tok=strtok(NULL,s);	   
		strcpy(nom,tok);
		tok=strtok(NULL,s);
		strcpy(mdp,tok);
		tok=strtok(NULL,s);
		nouveau = atoi(tok);
		printf("nom : %s et mdp :%s\n",nom,mdp);
		printf("nouveau : %d\n",nouveau);
		sprintf(requeteSQL,"select * from clients where login='%s'",nom);
		if (mysql_query(connexion,requeteSQL) != 0)
          {
            printf("Erreur de mysql_query: %s\n",mysql_error(connexion));
            exit(1);
          }
		if ((resultat = mysql_store_result(connexion)) == NULL)
          {
            printf("Erreur de mysql_store_result: %s\n",mysql_error(connexion));
            exit(1);
          }
         if((Tuple = mysql_fetch_row(resultat)) != NULL)
          {
          	if(nouveau==1)
          	{
                	sprintf(message,"Client deja existant");
                	status = 0;		                    
          	}
          	else
          	{
          		if(strcmp(nom,Tuple[1])==0)
                {
                	if(strcmp(mdp,Tuple[2])==0)
                	{
                		sprintf(message,"Bonjour %s",nom);
                		status = 1;
                	}
                	else
                	{
               			sprintf(message,"Mauvais mot de passe");
                		status = 0;
                	}
                }
          	}
            
          }
          else
          {
          	if(nouveau==1)
          	{
          		sprintf(requeteSQL,"INSERT INTO clients (id,login,password) VALUES ('%d','%s','%s');",(NULL,nom,mdp));
                status = 1;
          	}
          	else
          	{
                sprintf(message,"Mauvais nom d'utilisateur");
                status = 0;
          	}
          	
          }
          sprintf(communication,"LOGIN");
		  strcat(communication,s);
		  sprintf(sta,"%d",status);
		  strcat(communication,sta);
		  strcat(communication,s);
		  strcat(communication,message);
		   strcat(communication,s);
		  strcpy(reponse,communication);
		  if(status==1)
		  {
		  	ajoute(socket);
		  }
	
}
void FctConsult(char* requete,char * reponse)
{
	MYSQL_RES  *resultat;
  MYSQL_ROW  Tuple;
  char requeteSQL[100];
		char id[20];
		int idInt;
		tok=strtok(NULL,s);
		strcpy(id,tok);
		idInt=atoi(id);
		sprintf(requeteSQL,"select * from articles where id='%d'",idInt);
		if (mysql_query(connexion,requeteSQL) != 0)
          {
            printf("Erreur de mysql_query: %s\n",mysql_error(connexion));
            exit(1);
          }
		if ((resultat = mysql_store_result(connexion)) == NULL)
          {
            printf("Erreur de mysql_store_result: %s\n",mysql_error(connexion));
            exit(1);
          }
         sprintf(reponse,"CONSULT");
		 strcat(reponse,s);
         if((Tuple = mysql_fetch_row(resultat)) != NULL)
         {
         	char prix[20];
         	strcat(reponse,Tuple[0]); //id
         	strcat(reponse,s);
         	strcat(reponse,Tuple[1]); //intitule
         	strcat(reponse,s);
         	strcat(reponse,remplacerpoint(Tuple[2])); //prix
         	strcat(reponse,s);
         	strcat(reponse,Tuple[3]); //stock
         	strcat(reponse,s);
         	strcat(reponse,Tuple[4]); //image
         	strcat(reponse,s);
         	strcat(reponse,"\0");
         	printf("reponse : %s\n",reponse);
         }
         else
         {
         	strcat(reponse,"-1");
          	strcat(reponse,s);
          }
	
}

void FctAchat(char* requete,char* reponse,char* lArticle)
{
	MYSQL_RES  *resultat;
  MYSQL_ROW  Tuple;
  char requeteSQL[100];
  char listeArticle[100];
  int a = recupererNbrArticle(lArticle);
  printf("a : %d\n",a);
		if(10>a)
		{
			char id[20],sid[20],quant[20],squant[50],sprix[50],sintitule[50],c[200],communication[200];
			int idInt,quantite;
			strcpy(sid,"id:");
			strcpy(squant,"quant:");
			strcpy(sprix,"prix:");
			strcpy(sintitule,"intitule:");
			tok=strtok(NULL,s);
			strcpy(id,tok);
			printf("%s\n\n",id);
			idInt=atoi(id);
			tok=strtok(NULL,s);
			strcpy(quant,tok);
			quantite=atoi(quant);
			printf("quant = %d\n",quantite);
			sprintf(requeteSQL,"select * from articles where id='%d'",idInt);
			printf("select * from articles where id='%d'\n",idInt);
			if (mysql_query(connexion,requeteSQL) != 0)
	          {
	            printf("Erreur de mysql_query: %s\n",mysql_error(connexion));
	            exit(1);
	          }
			if ((resultat = mysql_store_result(connexion)) == NULL)
	          {
	            printf("Erreur de mysql_store_result: %s\n",mysql_error(connexion));
	            exit(1);
	          }
	          sprintf(reponse,"ACHAT");
			  strcat(reponse,s);
	         if((Tuple = mysql_fetch_row(resultat)) != NULL)
	         {
	         	if(quantite<=atoi(Tuple[3]))
	         	{
	         		sprintf(requeteSQL,"UPDATE articles SET stock = %d - %d where id=%d",atoi(Tuple[3]),quantite,idInt);
	         		

	         		if (mysql_query(connexion,requeteSQL) != 0)
	                {
	                  fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
	                  exit(1);
	                }
	                char prix[20];
	         			printf("avant creation trame\n");
					  strcat(reponse,Tuple[0]); //id
					  strcat(reponse,s);
					  strcat(reponse,Tuple[1]); //intitule
					  strcat(reponse,s);
					  strcpy(prix,remplacerpoint(Tuple[2]));
					  strcat(reponse,prix); //prix
					  strcat(reponse,s);
					  strcat(reponse,quant); //stock
					  strcat(reponse,s);
					  strcat(sid,Tuple[0]);
					  strcat(squant,quant);
					  strcat(sintitule,Tuple[1]);
					  strcat(sprix,prix);
					  sprintf(c,sid);
					  strcat(c,s);
					  strcat(c,sintitule);
					  strcat(c,s);
					  strcat(c,sprix);
					  strcat(c,s);
					  strcat(c,squant);
					  printf("c = %s\n",c);
					  ajouterArticle(lArticle,c);
					  printf("sors du ajouterArticle\n");
	         	}
	         	else
	         	{
	         		strcat(reponse,"-1");
	         		strcat(reponse,s);
	         		strcat(reponse,"Pas assez de stock");
	         		strcat(reponse,s);
	         	}
	         	 
	    }
	    else
	    {
	    	strcat(reponse,"-1");
	    	strcat(reponse,s);
	    	strcat(reponse,"Panier plein");
	    	strcat(reponse,s);	
	    }
	}

}

void FctCancel(char* requete,char* reponse,char* lArticle)
{
	
  printf("la requete dans le cancel : %s\n\n",requete);
	char requeteSQL[200]="";
		tok=strtok(requete,s);
		tok=strtok(NULL,s);
		char id[4];
		strcpy(id,tok);
		printf("lArticle dans FCTCANCEL : %s\n",lArticle);
		int a= suppArticle(lArticle,tok);
		printf("sortie du supp\n\n");
		sprintf(reponse,"CANCEL");
		strcat(reponse,s);
		if(a==1)
		{
			strcat(reponse,lArticle);
			
                           
		}
		else
			strcat(reponse,"-1");
		strcat(reponse,s);
		strcat(reponse,"\0");
		printf("reponse : %s\n",reponse);
	
}
void FctCaddie(char* requete, char* reponse,char* lArticle)
{
	printf("rentre dans caddie\n");
	sprintf(reponse,"CADDIE∕");
	strcat(reponse,lArticle);
	strcat(reponse,s);
}
int estPresent(int socket)
{
	int indice = -1;
	pthread_mutex_lock(&mutexClients);
	for(int i=0 ; i<nbclients ; i++)
	if (clients[i] == socket) { indice = i; break; }
	pthread_mutex_unlock(&mutexClients);
	return indice;
}
void ajoute(int socket)
{
	pthread_mutex_lock(&mutexClients);
	clients[nbclients] = socket;
	nbclients++;
	pthread_mutex_unlock(&mutexClients);
}
void retire(int socket)
{
	int pos = estPresent(socket);
	if (pos == -1) return;
	pthread_mutex_lock(&mutexClients);
	for (int i=pos ; i<=nbclients-2 ; i++)
	clients[i] = clients[i+1];
	nbclients--;
	pthread_mutex_unlock(&mutexClients);
}
char* remplacerpoint(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (str[i] == '.') {
            str[i] = ',';
        }
    }
    return str;
}
void ajouterArticle(char* lArticle,char *c) 
{
	printf("passe dans ajouterArticle\n");
	printf("(AJOUTARTICLE)contenu de lArticle : %s\n",lArticle);
    int nbr = recupererNbrArticle(lArticle);
    char listeArticle[100];
    if(nbr==0)
    {
    	sprintf(lArticle,"01$");
    	strcat(lArticle,"0/");
    	strcat(lArticle,c);
    	strcat(lArticle,dollar);
    }
    else
    {
    	 char nouveauNbrStr[3],test[10];
	    sprintf(nouveauNbrStr, "%02d", nbr+1); // Formatage du nombre sur 2 caractères
	    printf("moitie\n");
	    lArticle[0] = nouveauNbrStr[0];
	    lArticle[1] = nouveauNbrStr[1];
	    sprintf(test,"%d",nbr);
	    strcat(lArticle, test);
	    strcat(lArticle, s);
	    strcat(lArticle, c);
	    strcat(lArticle, dollar);
    }
    printf("la chaine d'article contient : %s\n", lArticle);
}
int recupererNbrArticle(char* lArticle)
{
	char nbrStr[3]; 
	int nbr;
	    strncpy(nbrStr, lArticle, 2);
	    nbrStr[2] = '\0'; 
	    nbr = atoi(nbrStr);
	printf("nbr dans recupererNbrArticle : %d\n",nbr);
    return nbr;
}
int suppArticle(char* lArticle,char* ind) 
{
	MYSQL_RES  *resultat;
  MYSQL_ROW  Tuple;
	printf("\n(SUPPARTICLE) lArticle = %s\n",lArticle);
    int a = recupererNbrArticle(lArticle);
    int indice;
    char cpy[500] = "",test[500]="",id[500]="",tmp[500]="",str[500]="",requeteSQL[500];
    char listeArticle[100];
    char quantite[10],place[2];
    int i=0;
    int b = 0; // Indique si un article a été supprimé
    strcpy(test,lArticle);
    strcpy(listeArticle, "");
    tok = strtok(test, dollar);
    printf("le token : %s\n",tok);
    tok = strtok(NULL, dollar);
    while (tok != NULL) 
    {
    	printf("le token %d: %s\n",i,tok);
    	i++;
    	strcpy(cpy,tok);
    	id[0]=cpy[5];
    	place[0]=cpy[0];

	    char* ptr = strstr(cpy,"quant:");
	    
	    if (ptr != NULL) 
	    {
	    	printf("AMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n");
	        // Si "quant:" est trouvé, avance le pointeur
	        ptr += strlen("quant:");
	        
	        int i = 0;
	        
	        while (*ptr != '\0' && *ptr != '/') {
	            quantite[i++] = *ptr;
	            ptr++;
	        }
	        printf("quantite : %s\n",quantite);
	    }
	    	printf("contenu de id :%s & contenu de place :%s\n",id,place);
        if(strcmp(ind,place)==0)
        {
        	printf("la chaine :%s\n",tok);
        	int idInt = atoi(id);
			sprintf(requeteSQL,"select * from articles where id='%d'",idInt);
			printf("select * from articles where id='%d'\n",idInt);
			if (mysql_query(connexion,requeteSQL) != 0)
	          {
	           
	            exit(1);
	          }
			if ((resultat = mysql_store_result(connexion)) == NULL)
	          {
	            printf("Erreur de mysql_store_result: %s\n",mysql_error(connexion));
	            exit(1);
	          }
	          if((Tuple = mysql_fetch_row(resultat)) != NULL)
	          {
	            int quant,quantBD,quanttotal;
	            char tmp[500];
	            sprintf(tmp,lArticle);
	            printf("id : %s\n",id);
	            quant=atoi(quantite);
	            printf("quant : %d\n",quant);
	            quantBD=atoi(Tuple[3]);
	            quanttotal = quant + quantBD;
	              sprintf(requeteSQL,"UPDATE articles SET stock = %d where id=%d",quanttotal,idInt);
	              printf("UPDATE articles SET stock = %d where id=%d",quanttotal,idInt+1);
	              if (mysql_query(connexion,requeteSQL) != 0)
	                {
	                   printf("Erreur de mysql_query: %s\n",mysql_error(connexion));
	                  exit(1);
	                }
	          }
        	printf("VRAIIIII \n");
            tok = strtok(NULL, dollar);
            printf("tok : %s\n",tok);
            if(tok!=NULL)
            {            	
            	while(tok!=NULL)
            	{
            		char t[5]="";
            		strncpy(t,tok,2);
            		sprintf(tmp,tok);
            		printf("le token %s\n",tmp);
            		if(t[1]=='/')
            		{
            			printf("rentre dans le if\n");
            			t[1]='\0';
            			indice = t[0]-'0';
            		}
            		else
            		{
            			indice = atoi(t);
            		}
            		printf("ptr = %s et indice = %d\n",t,indice);
            		indice--;
            		tmp[0]= indice +'0';
            		printf("tmp =%s\n",tmp);
            		strcat(str,tmp);
            		strcat(str,dollar);
            		str[strlen(str)]='\0';
            		printf("str : %s\n",str);
            		tmp[0] = '\0';
            		tok = strtok(NULL, dollar);            			
            	}
            }
            b = 1;
        }
        else 
        {
            strcat(str, tok);
            strcat(str, dollar);
        }
        
        tok = strtok(NULL, dollar);
    
    
	}
    if(tok!=NULL)
    {
    	strncpy(cpy,tok,1);
	    if(strcmp(ind,cpy))
	    {
	    	b==1;
	    }	
    }   
    if(b==1)
    {
    	char nouveauNbrStr[3];
    	sprintf(nouveauNbrStr, "%02d", a-1); // Formatage du nombre sur 2 caractères
	 	listeArticle[0]='\0'; 
	    listeArticle[0] = nouveauNbrStr[0];
	    listeArticle[1] = nouveauNbrStr[1];
	    listeArticle[2]='\0';
	    printf("ALLLO %s\n",listeArticle);
	    strcat(listeArticle,dollar);
	    printf("la liste article vaut %s\n",listeArticle);
	    strcat(listeArticle,str);
	    strcpy(lArticle,listeArticle);
    	strcat(lArticle,"\0");
    }
    else
    {
    	printf("rentre dans le else disant que rien n'a ete supp\n");
		listeArticle[0]='\0'; 
    	strcpy(lArticle,id);
    	strcat(lArticle,"\0");
    }
    printf("la liste = %s\n", lArticle);
    return b;

	
}
int rechercherQuantite(char* ind, char* chaine) 
{
    int quant = 0;
    char* tok = strtok(chaine, dollar);
    printf("(rechercherQuantite) tok = %s\n",tok);
    while (tok != NULL) 
    {
    	printf("la chaine normale = %s\n",tok);
        char t[5] = "";
        strncpy(t, tok, 1);
        printf("(rechercherQuantite) t = %s\n",t);
        if (strcmp(ind, t) == 0) 
        {
        	printf("rentre dans le if\n");
            char* ptr;
            char rech[30];
            sprintf(rech, "%s", tok);
            ptr = strstr(rech, "quant:");

            if (ptr != NULL) 
            {
                char q[3];
                int i = 0;

                while (ptr[i] != '/' && i < 3) 
                {
                    q[i] = ptr[i];
                    i++;
                }

                q[i] = '\0';
                quant = atoi(q);
                ptr =NULL;
            }
        }

        tok = strtok(NULL, dollar);
    }

    return quant;
}