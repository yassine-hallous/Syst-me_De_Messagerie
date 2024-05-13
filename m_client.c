#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "client.h"

#define CONNECTION_HOST "127.0.0.1"
#define LISTENTING_PORT 5094
#define BUFFER_SIZE 1024

/**********************************/

int menu_message_c(int socketFD) {
    int n , i  ;
    message m ;
    do {
	    n = receiveint(socketFD);
	    if ( n == 0 ) {
	    	printf("\n\nVous n'avez aucune Conversation pour le moment.\n\n");
	    	printf("Tapez : 1 -  pour Ajouter un destinataire .\n"
	    	       "        0 -  pour Quitter .\n");
	    	n = reessayer() ;
	    	sendint(socketFD , n ) ;
	    	if ( n ) {
	    		ajouter_c(socketFD);
	    	}
	    	else 
	    		return 0 ;
	    }
	    else if ( n == 1) {
			printf("\n\nVoici vos conversations :\n\n");	
			i = 0 ;
			while(receiveint(socketFD)){
				receivemessage(socketFD , m.destinataire);
			  	if (printf("%d - %s\n",++i,m.destinataire) > 0)
			    		sendint(socketFD , 1 );
			}
			printf("\nTapez : le numero du destinataire -  pour Acceder a la conversation .\n"
			       "                 -1               -  pour Ajouter un destinataire .\n"
			       "                 -2               -  pour Supprimer une conversation .\n"
			       "                  0               -  pour Quitter .\n"); 
			 n = ecrire( -2 , i ) ;
			 sendint(socketFD , n);
			 if ( n > 0 )
			 	conversation_c(socketFD , n );
			 else if ( n == -1 )
			   	ajouter_c(socketFD);
			 else if ( n == -2 )
			 	supprimer_destinataire_c(socketFD , i);
			 else if ( n == 0 )
			    	return 0 ;
	     }
     }while(1);
     return 0 ;
}

/**********************************/

int conversation_c( int socketFD , int n ) {
	int b , i ; 
	char destinataire[50] ;
	char message[1000] ;
	do {
		b = receiveint(socketFD);
		if ( b == 0 ) {
			printf("\n\nCette conversation est vide. Commencez à écrire !\n\n");
			printf("Tapez : 1 - pour Envoyer un message .\n"
			       "        0 - pour Quitter .\n");
			b = reessayer();
			sendint(socketFD , b ) ;
			if ( b ) 
				envoyer_c(socketFD);
			else 
				return 0 ;
		}
		else if ( b == 1 ) {
			receivemessage(socketFD , destinataire);
			printf("\n\nConversation avec %s :\n\n",destinataire);
			sendint(socketFD , 1);
			i = 0 ;
			while(receiveint(socketFD)){
				receivemessage(socketFD , message);
			  	if (printf("%d - %s\n",++i,message) > 0)
			    		sendint(socketFD , 1 );
			}
			printf("\nTapez : 1 -  pour Envoyer un message .\n"
			       "        2 -  pour Supprimer un message .\n"
			       "        0 -  pour Quitter .\n");
			 n = ecrire( 0 , 2 ) ;
			 sendint(socketFD , n);
			 if ( n == 1 ) {
			 	envoyer_c(socketFD);
			 	receiveint(socketFD);
			 }
			 else if ( n == 2 ) {
			 	supprimer_message_c(socketFD , i);
			 }
			 else if ( n == 0 ) {
			 	return 0 ;
			 }
		}
	}while(1);
	return 0 ;
}

/**********************************/

int envoyer_c( int socketFD ) {
	char message[1000];
	int b = 0 ;
	do {
		printf("\nEcrire votre message ( max 1000 lettres ) : ");
		fgets(message, sizeof(message), stdin);
		size_t len = strlen(message);
		if (len > 0 && message[len - 1] == '\n') {
			message[len - 1] = '\0';
		}
		if (strlen(message) == 0) {
			printf("Le message est vide. Aucun envoi n'est effectué.\n");
			b++;
		}
		else if (strlen(message) > 1000) {
			printf("Le message est trop long (plus de 1000 caractères). Aucun envoi n'est effectué.\n");
			b++;
		}
	}while( b == 1 ) ;
	sendmessage(socketFD , message);
	return 0 ;
}

/**********************************/

void supprimer_message_c(int socketFD , int i ){
	int n ;
	printf("\nVeuillez entrer le numéro du message que vous souhaitez supprimer : ");
	n = ecrire ( 1 , i );
	sendint(socketFD , n ) ;
	printf("\nTapez : 1 - Pour confirmer la suppression du message .\n"
	       "        0 - Pour annuler la suppression du message .\n\n") ;
	n = reessayer();
	sendint(socketFD , n );
	if ( n ) 
		printf("\nLa suppression du message a été effectuée avec succès.\n\n");
	else 
		printf("\nL'annulation de la suppression du message\n\n");
}

/**********************************/

int ajouter_c(int socketFD ) {
	message m ;
	int b = 0;
	do {
		m_destinataire(m.destinataire);
		sendmessage(socketFD , m.destinataire) ;
		b = receiveint(socketFD ) ;
 		if ( b == 0 ){
 			printf("Le nom de destinataire n'existe pas ou il est deja dans votre liste de conversation .\n");
 			printf("\nTapez : 1 - pour reessayer .\n"
			       "        0 - pour quitter .\n\n");
		        b = reessayer();
		        sendint(socketFD , b);
		        if ( b == 0 )
		        	return 0 ;
		        b = 0 ;
 		}
 	}while( b == 0 ) ; 
 	printf("\nVous avez ajouter votre destinataire avec succes.\n");
	return 0 ;
}

/**********************************/

void supprimer_destinataire_c(int socketFD , int i ){
	int n ;
	printf("\nVeuillez entrer le numéro du destinataire que vous souhaitez supprimer : ");
	n = ecrire ( 1 , i );
	sendint(socketFD , n ) ;
	printf("\nTapez : 1 - Pour confirmer la suppression du destinataire .\n"
	       "        0 - Pour annuler la suppression du destinataire .\n\n") ;
	n = reessayer();
	sendint(socketFD , n );
	if ( n ) 
		printf("\nLa suppression du destinataire a été effectuée avec succès.\n\n");
	else 
		printf("\nL'annulation de la suppression du destinataire\n\n");
}

/**********************************/

char* m_destinataire ( char user[] ){
    int i , b ;
    do{
        b = 0 ;
        printf("\n\nVeuillez saisir le nom d'utilisateur du destinataire : ");
        scanf("%s",user);
        if(strlen(user) > 50){
            printf("Nom du destinataire trop long.\n");
        } 
        else if (strlen(user) == 0){
            printf("Nom du destinataire vide .\n");
        }
        else {
            for (i = 0; (user)[i] != '\0'; i++) {
                if( isdigit(user[0]) || user[0] == '_' ){
                    printf("Nom du destinataire ne doit pas commencer par des chiffres ou par '_' .\n");
                    b++;
                    break;
                }
                if (!isalnum(user[i]) && (user)[i] != '_'){
		        printf("Nom du destinataire ne doit contenir que des lettres et des chiffres et des '_' .\n");
		        b++;
		        break;
                }
            }
        }
    }while(strlen(user) == 0 || strlen(user) > 50 || b );
    return user ;
}

/**********************************/



	
	

























