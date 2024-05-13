#ifndef COMPTE_H
#define COMPTE_H

/**********************************/

typedef enum {
    false,
    true
} bool ;

/**********************************/

typedef struct {
    char username[50] , password[50] ;
}compte;

typedef struct {
	char destinataire[50] ;
}message;

/**********************************/

int accueil(void);
int menu(void) ;
void cree(void);
char* c_user(char user[]);
char* c_pass(char pass[]);
int reessayer();
int ecrire ( int d , int f );
char* user_pass (char username[] , char password[] , char ligne[] ) ;
void sendmessage(int socketFD, const char message[] );
void sendint(int socketFD , int n );
char* receivemessage(int socketFD , char buffer[] ) ;
int receiveint(int socketFD ) ;
void cree_c(int socketFD ) ;
int connecter_c(int socketFD ) ;
int modification_c ( int socketFD ) ;
int supprimer_c ( int socketFD ) ; 
int menu_message_c(int socketFD) ;
int ajouter_c(int socketFD );
char* m_destinataire ( char user[] );
void supprimer_destinataire_c(int socketFD, int i);
int conversation_c( int socketFD , int n ) ;
int envoyer_c( int socketFD);
void supprimer_message_c(int socketFD , int i );

/**********************************/

#endif
