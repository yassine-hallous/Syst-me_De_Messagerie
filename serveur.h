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

int verifier_user(char user[] , int l ) ;
void sendmessage(int socketFD, const char message[] );
void sendint(int socketFD , int n );
char* receivemessage(int socketFD , char buffer[] ) ;
int receiveint(int socketFD ) ;
void cree_s ( int connectedSocketFD );
int connecter_s (int connectedSocketFD , int *l ) ;
int modification_s (int connectedSocketFD , int l  ) ;
int supprimer_s ( int connectedSocketFD , int l ) ;
void menu_message_s(int conncetedSocketFD , int l ) ;
int ajouter_s(int connectedSocketFD , char user[] , int l );
int verifier_destinataire(char destinataire[] , char fichier[] , int l );
char* fichier_c(char ligne[] , char fichier[] ) ;
char* user_des(char username[] , int l );
void supprimer_destinataire_s ( int connectedSocketFD , char fichier[] , int l );
int conversation_s(int connectedSocketFD , char fichier[] , int n , int l);
char* user_destinataire( char test[] ,  char user[] , char destinataire[] );
void envoyer_s( int connectedSocketFD  , char user_f[] , char dest_f[] , char user[] );
char* time_s(char time_string[] , size_t size) ;
void supprimer_message_s ( int connectedSocketFD , char user_f[] , char dest_f[] ) ;
char* user_conversation(char username[] , char fichier[]);
int verifier_compte(char test[] , int l);
char* user_pass (char username[] , char password[] , char ligne[] );
void modifier_compte(char n_ligne[] , int l ) ;
void modifier_user_conversation( char user_n[] , int l ) ;
void modifier_user_destinataire( char ligne[] ,char user_a[] , char user_n[] ) ;
void supprimer_compte( int l );
void supprimer_user_conversation(int l);
void supprimer_user_destinataire(char fichier[] , char username[] );


/**********************************/

#endif
