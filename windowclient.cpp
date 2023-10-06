#include "windowclient.h"
#include "ui_windowclient.h"
#include "Socket/TCP.h"
#include <QMessageBox>
#include <signal.h>
#include <string>
using namespace std;

extern WindowClient *w;


#define NUMARTICLEMAX 21
#define REPERTOIRE_IMAGES "images/"
int sClient=-1;
bool logged;
void HandlerSIGINT(int s);
void mettreAJourArticle(char* c);
int recupererNbrArticle(char* c);

char idClient[3];
int NumArticle=0;
float totalCaddie = 0.0;
const char s[2]="/";
const char dollar[2]="$";
char* tok;
 
WindowClient::WindowClient(QWidget *parent) : QMainWindow(parent), ui(new Ui::WindowClient)
{
    ui->setupUi(this);

    // Configuration de la table du panier (ne pas modifer)
    ui->tableWidgetPanier->setColumnCount(3);
    ui->tableWidgetPanier->setRowCount(0);
    QStringList labelsTablePanier;
    labelsTablePanier << "Article" << "Prix à l'unité" << "Quantité";
    ui->tableWidgetPanier->setHorizontalHeaderLabels(labelsTablePanier);
    ui->tableWidgetPanier->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetPanier->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetPanier->horizontalHeader()->setVisible(true);
    ui->tableWidgetPanier->horizontalHeader()->setDefaultSectionSize(160);
    ui->tableWidgetPanier->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetPanier->verticalHeader()->setVisible(false);
    ui->tableWidgetPanier->horizontalHeader()->setStyleSheet("background-color: lightyellow");

    ui->pushButtonPayer->setText("Confirmer achat");
    setPublicite("!!! Bienvenue sur le Maraicher en ligne !!!");

    struct sigaction A;
  A.sa_flags = 0;
  sigemptyset(&A.sa_mask);
  A.sa_handler = HandlerSIGINT;
  if (sigaction(SIGINT,&A,NULL) == -1)
  {
    perror("Erreur de sigaction");
    exit(1);
  }
   FILE *fichier = fopen("config.txt", "r");
    if (fichier == NULL) {
        perror("Impossible d'ouvrir le fichier de configuration");
        exit(1);
    }

    char ligne[100];
    int port_achat = -1; 
    char IPSERVEUR[15];
     while (fgets(ligne, sizeof(ligne), fichier) != NULL) 
     {
        if (sscanf(ligne, "PORT_ACHAT = %d", &port_achat) == 1)
        {
          
        } 
      else if (sscanf(ligne, "IPSERVEUR = %s", IPSERVEUR) == 1) 
      {
      }
    }


    // Fermez le fichier de configuration
    fclose(fichier);

    // Vérifiez si le port a été lu avec succès
    if (port_achat != -1) {
        printf("Numéro de port d'achat lu depuis le fichier de configuration : %d\n", port_achat);
    } else {
        printf("Impossible de lire le numéro de port d'achat depuis le fichier de configuration.\n");
    }
    printf("Adresse IP du serveur lue depuis le fichier de configuration : %s\n", IPSERVEUR);
    // Connexion sur le serveur
  if ((sClient = ClientSocket(IPSERVEUR,port_achat)) == -1)
  {
    perror("Erreur de ClientSocket");
    exit(1);
  }
  printf("\nle putain de sClient vaut : %d\n",sClient);
  if(sClient!=0)
    printf("Connecte sur le serveur!!!!.\n");
}

WindowClient::~WindowClient()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles : ne pas modifier /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setNom(const char* Text)
{
  if (strlen(Text) == 0 )
  {
    ui->lineEditNom->clear();
    return;
  }
  ui->lineEditNom->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* WindowClient::getNom()
{
  strcpy(nom,ui->lineEditNom->text().toStdString().c_str());
  return nom;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setMotDePasse(const char* Text)
{
  if (strlen(Text) == 0 )
  {
    ui->lineEditMotDePasse->clear();
    return;
  }
  ui->lineEditMotDePasse->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* WindowClient::getMotDePasse()
{
  strcpy(motDePasse,ui->lineEditMotDePasse->text().toStdString().c_str());
  return motDePasse;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setPublicite(const char* Text)
{
  if (strlen(Text) == 0 )
  {
    ui->lineEditPublicite->clear();
    return;
  }
  ui->lineEditPublicite->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setImage(const char* image)
{
  // Met à jour l'image
  char cheminComplet[80];
  sprintf(cheminComplet,"%s%s",REPERTOIRE_IMAGES,image);
  QLabel* label = new QLabel();
  label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  label->setScaledContents(true);
  QPixmap *pixmap_img = new QPixmap(cheminComplet);
  label->setPixmap(*pixmap_img);
  label->resize(label->pixmap()->size());
  ui->scrollArea->setWidget(label);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WindowClient::isNouveauClientChecked()
{
  if (ui->checkBoxNouveauClient->isChecked()) return 1;
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setArticle(const char* intitule,float prix,int stock,const char* image)
{
  ui->lineEditArticle->setText(intitule);
  if (prix >= 0.0)
  {
    char Prix[20];
    sprintf(Prix,"%.2f",prix);
    ui->lineEditPrixUnitaire->setText(Prix);
  }
  else ui->lineEditPrixUnitaire->clear();
  if (stock >= 0)
  {
    char Stock[20];
    sprintf(Stock,"%d",stock);
    ui->lineEditStock->setText(Stock);
  }
  else ui->lineEditStock->clear();
  setImage(image);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WindowClient::getQuantite()
{
  return ui->spinBoxQuantite->value();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setTotal(float total)
{
  if (total >= 0.0)
  {
    char Total[20];
    sprintf(Total,"%.2f",total);
    ui->lineEditTotal->setText(Total);
  }
  else ui->lineEditTotal->clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::loginOK()
{
  ui->pushButtonLogin->setEnabled(false);
  ui->pushButtonLogout->setEnabled(true);
  ui->lineEditNom->setReadOnly(true);
  ui->lineEditMotDePasse->setReadOnly(true);
  ui->checkBoxNouveauClient->setEnabled(false);

  ui->spinBoxQuantite->setEnabled(true);
  ui->pushButtonPrecedent->setEnabled(true);
  ui->pushButtonSuivant->setEnabled(true);
  ui->pushButtonAcheter->setEnabled(true);
  ui->pushButtonSupprimer->setEnabled(true);
  ui->pushButtonViderPanier->setEnabled(true);
  ui->pushButtonPayer->setEnabled(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::logoutOK()
{
  ui->pushButtonLogin->setEnabled(true);
  ui->pushButtonLogout->setEnabled(false);
  ui->lineEditNom->setReadOnly(false);
  ui->lineEditMotDePasse->setReadOnly(false);
  ui->checkBoxNouveauClient->setEnabled(true);

  ui->spinBoxQuantite->setEnabled(false);
  ui->pushButtonPrecedent->setEnabled(false);
  ui->pushButtonSuivant->setEnabled(false);
  ui->pushButtonAcheter->setEnabled(false);
  ui->pushButtonSupprimer->setEnabled(false);
  ui->pushButtonViderPanier->setEnabled(false);
  ui->pushButtonPayer->setEnabled(false);

  setNom("");
  setMotDePasse("");
  ui->checkBoxNouveauClient->setCheckState(Qt::CheckState::Unchecked);

  setArticle("",-1.0,-1,"");

  w->videTablePanier();
  totalCaddie = 0.0;
  w->setTotal(-1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles Table du panier (ne pas modifier) /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::ajouteArticleTablePanier(const char* article,float prix,int quantite)
{
    char Prix[20],Quantite[20];

    sprintf(Prix,"%.2f",prix);
    sprintf(Quantite,"%d",quantite);

    // Ajout possible
    int nbLignes = ui->tableWidgetPanier->rowCount();
    nbLignes++;
    ui->tableWidgetPanier->setRowCount(nbLignes);
    ui->tableWidgetPanier->setRowHeight(nbLignes-1,10);

    QTableWidgetItem *item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(article);
    ui->tableWidgetPanier->setItem(nbLignes-1,0,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(Prix);
    ui->tableWidgetPanier->setItem(nbLignes-1,1,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(Quantite);
    ui->tableWidgetPanier->setItem(nbLignes-1,2,item);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::videTablePanier()
{
    ui->tableWidgetPanier->setRowCount(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WindowClient::getIndiceArticleSelectionne()
{
    QModelIndexList liste = ui->tableWidgetPanier->selectionModel()->selectedRows();
    if (liste.size() == 0) return -1;
    QModelIndex index = liste.at(0);
    int indice = index.row();
    return indice;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions permettant d'afficher des boites de dialogue (ne pas modifier ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::dialogueMessage(const char* titre,const char* message)
{
   QMessageBox::information(this,titre,message);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::dialogueErreur(const char* titre,const char* message)
{
   QMessageBox::critical(this,titre,message);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// CLIC SUR LA CROIX DE LA FENETRE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::closeEvent(QCloseEvent *event)
{

  exit(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions clics sur les boutons ////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonLogin_clicked()
{
  if(strcmp(getNom(),"")==0 || strcmp(getMotDePasse(),"")==0)
  {
    dialogueErreur("Erreur login", "veuillez completer les champs");
  }
  else
  {
    if(strlen(getNom())>=20|| strlen(getMotDePasse())>=20)
    {
      dialogueErreur("Erreur login", "veuillez completer les champs avec moins de 20 caracteres");
    }
    else
    {

    char message[80],reponse[80],opt[20];
   
  sprintf(message,"LOGIN");
  strcat(message,s);
  strcat(message,getNom());
  strcat(message,s);
  strcat(message,getMotDePasse());
  strcat(message,s);
  if(isNouveauClientChecked())
    strcat(message,"1");
  else
    strcat(message,"0");
  printf("message : %s\n",message);
  int nbEcrits;
  printf("\n dans le login, le sClient vaut : %d\n",sClient);
  if ((nbEcrits = Send(sClient,message,strlen(message))) == -1)
  {
    perror("Erreur de Send");
    exit(1);
  }
  printf("NbEcrits = %d\n",nbEcrits);
  printf("\n\nEcrit= --%s--\n",message);
  
  //reception de confirmation connexion

  printf("attente de la confirmation\n");
   if (Receive(sClient, reponse) < 0)
    {
        perror("Erreur de Receive");
        //close(sClient);
    }
    printf("reponse : %s\n",reponse);
      tok = strtok(reponse, s);
  strcpy(opt, tok);
  if (strcmp(opt, "LOGIN") == 0)
    {
      
      tok=strtok(NULL,s);
      strcpy(opt, tok);
       tok=strtok(NULL,s);
      if(strcmp(opt,"1")==0)
      {
        dialogueMessage("connexion",tok);
        puts(tok);
        w->loginOK();
        logged = true;
        tok=strtok(NULL,s);
        strcpy(idClient,tok);
        sprintf(message,"CONSULT");
        strcat(message,s);
        strcat(message,"1");
         strcat(message,s);
         strcat(message,"\0");
        if (Send(sClient,message,strlen(message)) == -1)
        {
          perror("Erreur de Send");
          exit(1);
        }
         if (Receive(sClient, reponse) < 0)
        {
            perror("Erreur de Receive");
            //close(sClient);
        }
        printf("(CLIENT) reponse : %s\n",reponse);
        tok=strtok(reponse,s);
         strcpy(opt, tok);
         if (strcmp(opt, "CONSULT") == 0)
        {
          char id[3],intitule[20],prix[10],stock[10],image[30];
          int  sto;
          float pri;
          tok=strtok(NULL,s);
          strcpy(id, tok);
          NumArticle = atoi(id);
          tok=strtok(NULL,s);
          strcpy(intitule, tok);
          tok=strtok(NULL,s);
          strcpy(prix, tok);

          pri=atof(prix);
          tok=strtok(NULL,s);
          strcpy(stock, tok);
          sto=atoi(stock);
          tok=strtok(NULL,s);
          strcpy(image, tok);
          w->setArticle(intitule,pri,sto,image);
        }
      }
      else
      {
        dialogueErreur("connexion",tok);
        puts(tok);
        logged= false;
      }
      
    }
  }
   
  
  }


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonLogout_clicked()
{
  char m[500],opt[20],cpy[500];
  if(totalCaddie>0)
  {
    sprintf(m,"CANCEL_ALL");
    strcat(m,s);
     if ( Send(sClient,m,strlen(m)) == -1)
      {
        perror("Erreur de Send");
        exit(1);
      } 
      m[0]='\0';
       if (Receive(sClient, m) < 0)
        {
            perror("Erreur de Receive");
        }
        strcpy(cpy,m);
        printf("la chaine dans supp : %s\n",m);
          tok=strtok(m,s);
         strcpy(opt, tok);
         if (strcmp(opt, "CADDIE") == 0)
        {      
           w->dialogueMessage("Suppression","suppression reussie");
           //tok=strtok(NULL,s);

           mettreAJourArticle(cpy); 
          
        }
        else
        {
          w->dialogueErreur("Suppression","suppression impossible");
        }  
  }
  printf("envoi de logout\n");
  sprintf(m,"LOGOUT");
   strcat(m,s);
     if (Send(sClient,m,strlen(m)) == -1)
    {
      perror("Erreur de Send");
      exit(1);
    }
    m[0]='\0';
  if(Receive(sClient,m)<0)
   {
    perror("Erreur de Receive");
   }

    tok=strtok(m,s);
    if(strcmp(tok,"LOGOUT")==0)
    {
      w->logoutOK();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonSuivant_clicked()
{
  if(NumArticle<NUMARTICLEMAX)
    NumArticle++;
  char m[50],reponse[200],article[10],opt[20];
  int nbEcrits;
  sprintf(m,"CONSULT");
  strcat(m,s);
  sprintf(article,"%d",NumArticle);
  strcat(m,article);
   strcat(m,s);
    if ((nbEcrits = Send(sClient,m,strlen(m))) == -1)
    {
      perror("Erreur de Send");
      exit(1);
    }
      if (Receive(sClient, reponse) < 0)
        {
            perror("Erreur de Receive");
            //close(sClient);
        }
        printf("(CLIENT) reponse : %s\n",reponse);
        tok=strtok(reponse,s);
         strcpy(opt, tok);
         if (strcmp(opt, "CONSULT") == 0)
        {
          char id[3],intitule[20],prix[10],stock[10],image[30];
          int  sto;
          float pri;
          tok=strtok(NULL,s);
          strcpy(id, tok);
          NumArticle = atoi(id);
          tok=strtok(NULL,s);
          strcpy(intitule, tok);
          tok=strtok(NULL,s);
          strcpy(prix, tok);
          pri=atof(prix);
          tok=strtok(NULL,s);
          strcpy(stock, tok);
          sto=atoi(stock);
          tok=strtok(NULL,s);
          strcpy(image, tok);
          w->setArticle(intitule,pri,sto,image);
        }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonPrecedent_clicked()
{
  if(NumArticle>1)
    NumArticle--;
  char m[50],opt[20],article[10],reponse[200];
  int nbEcrits;
  sprintf(m,"CONSULT");
  strcat(m,s);
  sprintf(article,"%d",NumArticle);
  strcat(m,article);
   strcat(m,s);
    if ((nbEcrits = Send(sClient,m,strlen(m))) == -1)
    {
      perror("Erreur de Send");
      exit(1);
    }
      if (Receive(sClient, reponse) < 0)
        {
            perror("Erreur de Receive");
            //close(sClient);
        }
        printf("(CLIENT) reponse : %s\n",reponse);
        tok=strtok(reponse,s);
         strcpy(opt, tok);
         if (strcmp(opt, "CONSULT") == 0)
        {
          char id[3],intitule[20],prix[10],stock[10],image[30];
          int  sto;
          float pri;
          tok=strtok(NULL,s);
          strcpy(id, tok);
          NumArticle = atoi(id);
          tok=strtok(NULL,s);
          strcpy(intitule, tok);
          tok=strtok(NULL,s);
          strcpy(prix, tok);

          pri=atof(prix);
          tok=strtok(NULL,s);
          strcpy(stock, tok);
          sto=atoi(stock);
          tok=strtok(NULL,s);
          strcpy(image, tok);
          w->setArticle(intitule,pri,sto,image);
        }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonAcheter_clicked()
{
  if(getQuantite()==0)
  {
    dialogueErreur("Erreur achat", "veuillez selectionner une quantite valide");
   
  }
  else
  {
   char m[500],IP[20],Quantite[20],article[10],opt[20];
  
  int nbEcrits;
  sprintf(m,"ACHAT");
  strcat(m,s);
  sprintf(article,"%d",NumArticle);
  strcat(m,article);
  strcat(m,s);
  sprintf(Quantite,"%d",getQuantite());
  strcat(m,Quantite);
   strcat(m,s);
   printf("affichage du message : %s\n\n",m);
    if ((nbEcrits = Send(sClient,m,strlen(m))) == -1)
    {
      perror("Erreur de Send");
      exit(1);
    }
    printf("avant le Receive\n");
    m[0]='\0';
    if (Receive(sClient, m) < 0)
        {
            perror("Erreur de Receive");
            //close(sClient);
        }
        printf("(CLIENT) reponse : %s\n",m);
        tok=strtok(m,s);
         strcpy(opt, tok);
         printf("opt : %s\n",opt);

         if (strcmp(opt,"CADDIE") == 0)
        {
          
          tok=strtok(NULL,s);
          int nbr = atoi(tok);
          printf("nbr : %d",nbr);
                    
          w->dialogueMessage("Achat", "Vous avez achete avec succes ");
          w->videTablePanier();
          totalCaddie=0.0;
          char id[3],intitule[20],prix[10],stock[10];
          int  sto;
          float pri;
          tok=strtok(NULL,s);
          for(int i=0;i<nbr;i++)
          {
            
            tok=strtok(NULL,s);
            strcpy(id, tok);
            NumArticle = atoi(id);
            tok=strtok(NULL,s);
            strcpy(intitule, tok);
            tok=strtok(NULL,s);
            strcpy(prix, tok);
            pri=atof(prix);
            tok=strtok(NULL,s);
            strcpy(stock, tok);
            sto=atoi(stock);
            w->ajouteArticleTablePanier(intitule,pri,sto);
            totalCaddie+=sto*pri;
            w->setTotal(totalCaddie);
          }
        }
        else
        {

          tok=strtok(NULL,s);
          tok=strtok(NULL,s);
          w->dialogueMessage("Achat",tok);
      }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonSupprimer_clicked()
{
  char m[500],cpy[500],id[20],Quantite[20],article[10],opt[20];
  if(getIndiceArticleSelectionne()==-1)
    {
        dialogueErreur("Erreur suppression", "veuillez selectionner un article");
    }
    else
    {
      sprintf(m,"CANCEL");
      strcat(m,s);
      sprintf(id,"%d",getIndiceArticleSelectionne());
      strcat(m,id);
      strcat(m,s);
      printf("le supp dans windowclient : %s\n",m);
       if ((Send(sClient,m,strlen(m))) == -1)
      {
        perror("Erreur de Send");
        exit(1);
      }
      m[0]='\0';
       if (Receive(sClient, m) < 0)
        {
            perror("Erreur de Receive");
            //close(sClient);
        }
        strcpy(cpy,m);
        printf("la chaine dans supp : %s\n",m);
          tok=strtok(m,s);
         strcpy(opt, tok);
         if (strcmp(opt, "CADDIE") == 0)
        {          
           w->dialogueMessage("Suppression","suppression reussie");
           mettreAJourArticle(cpy); 
          
        }
        else
        {
          w->dialogueErreur("Suppression","suppression impossible");
        }

    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonViderPanier_clicked()
{
  if(totalCaddie==0.0)
  {
    dialogueErreur("Erreur suppression", "panier vide");
  }
  else
  {
    char m[500],cpy[500],id[20],Quantite[20],article[10],opt[20];
 
      sprintf(m,"CANCEL_ALL");
      strcat(m,s);
      strcat(m,"\0");
       if ((Send(sClient,m,strlen(m))) == -1)
      {
        perror("Erreur de Send");
        exit(1);
      }
      m[0]='\0';
       if (Receive(sClient, m) < 0)
        {
            perror("Erreur de Receive");
        }
        strcpy(cpy,m);
        printf("la chaine dans supp : %s\n",m);
          tok=strtok(m,s);
         strcpy(opt, tok);
         if (strcmp(opt, "CADDIE") == 0)
        {      
           w->dialogueMessage("Suppression","suppression reussie");
           //tok=strtok(NULL,s);

           mettreAJourArticle(cpy); 
          
        }
        else
        {
          w->dialogueErreur("Suppression","suppression impossible");
        }  
  }
  
    
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonPayer_clicked()
{
  if(totalCaddie==0.0)
  {
    w->dialogueErreur("paiement","panier vide");
  }
  else
  {
    char m[500],cpy[500],var[10];
    printf("PASSE PAR PAYER\n");
    sprintf(m,"CONFIRMER");
   strcat(m,s);
    strcat(m,idClient);
    strcat(m,s); 
   sprintf(var,"%f",totalCaddie);
   strcat(m,var);
   strcat(m,s);
    if ((Send(sClient,m,strlen(m))) == -1)
      {
        perror("Erreur de Send");
        exit(1);
      }
      m[0]='\0';
       if (Receive(sClient, m) < 0)
        {
            perror("Erreur de Receive");
        }
        printf("la chaine dans payer : %s\n",m);
        strcpy(cpy,m);
          tok=strtok(m,s);
         if (strcmp(tok, "CONFIRMER") == 0)
        {      
          char a[30]="numero de la facture = ";
          tok=strtok(NULL,s);
          strcat(a,tok);
           w->dialogueMessage("Facture",a);          
        }
      sprintf(m,"CADDIE");
      strcat(m,s);
      strcat(m,"\0");
       if ((Send(sClient,m,strlen(m))) == -1)
      {
        perror("Erreur de Send");
        exit(1);
      }
      m[0]='\0';
       if (Receive(sClient, m) < 0)
        {
            perror("Erreur de Receive");
        }
        strcpy(cpy,m);
        printf("la chaine dans supp : %s\n",m);
          tok=strtok(m,s);
         if (strcmp(tok, "CADDIE") == 0)
        {      
           w->dialogueMessage("Suppression","suppression reussie");
           //tok=strtok(NULL,s);

           mettreAJourArticle(cpy); 
           w->setTotal(totalCaddie);
          
        }
        else
        {
          w->dialogueErreur("Suppression","suppression impossible");
        }
  
  }
  
}

//***** Fin de connexion ********************************************
void HandlerSIGINT(int s)
{
  printf("\nArret du client.\n");
  //SMOP_Logout();
  close(sClient);
  exit(0);
}

void mettreAJourArticle(char* c)
{
  char nobr[3],id[3],intitule[50],inti[50],prix[30],pr[30],stock[30],st[30];
          int  sto;
          float pri;
  printf("rentre dans le majarticle : %s\n",c);
  totalCaddie=0.0;


  tok=strtok(c,s);
  
  tok=strtok(NULL,s);
  sprintf(nobr,tok);
  printf("tok = %s\n",tok);
 
  int nbr = atoi(nobr);
  printf("nbr = %d\n\n\n",nbr);
  w->videTablePanier();
  
    for (int i = 0; i < nbr; i++) {
    tok = strtok(NULL, s);//pour la place dans panier
    tok = strtok(NULL, s);//id
    tok = strtok(NULL, s); //pour l'intitule

    if(tok!=NULL)
    {
      sprintf(inti,tok);
    }
    tok = strtok(NULL, s);
    if(tok!=NULL)
    {
      sprintf(prix,tok);
      pri=atof(prix);
    }

    tok = strtok(NULL, dollar);
    if(tok!=NULL)
    {
      sprintf(stock,tok);
      sto=atoi(stock);
    }

    printf("apres\n");
    totalCaddie += sto * pri;
    w->setTotal(totalCaddie);
    printf("inti : %s & prix = %f & stock = %d\n",inti,pri,sto);
    w->ajouteArticleTablePanier(inti, pri, sto);
    

}

}
void Echange(char* requete, char* reponse)
{
  int nbEcrits, nbLus;
  // ***** Envoi de la requete ****************************
  if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
  {
    perror("Erreur de Send");close(sClient);
    exit(1);
  }
  // ***** Attente de la reponse **************************
  if ((nbLus = Receive(sClient,reponse)) < 0)
  {
    perror("Erreur de Receive");
    close(sClient);
    exit(1);
  }
}
int recupererNbrArticle(char * listeArticle)
{
  char nbrStr[3]; 
    strncpy(nbrStr, listeArticle, 2);
    nbrStr[2] = '\0'; 
    int nbr = atoi(nbrStr);
    return nbr;
}