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

void sendmessage(int socketFD, const char message[] ) {
    	ssize_t sentBytes = send(socketFD, message, strlen(message), 0);
    	if (sentBytes == -1) {
        	fprintf(stderr, "(CLIENT) Echec d'envoi de message au serveur\n");
        	exit(1);
    	}	
}

/**********************************/

void sendint(int socketFD , int n ) {
	int network_int = htonl(n);
	ssize_t sentBytes = send(socketFD, &network_int , sizeof(int) , 0);
    	if (sentBytes == -1) {
        	fprintf(stderr, "(CLIENT) Echec d'envoi de message au serveur\n");
        	exit(1);
    	}
}

/**********************************/

char* receivemessage(int socketFD , char buffer[] ) {
        int receivedBytes = recv(socketFD , buffer , BUFFER_SIZE , 0);
        if( receivedBytes == -1 ){
        	fprintf( stderr, "(CLIENT) Echec de reception du message du serveur\n" );
        	exit(1);
    	}
    	buffer[receivedBytes] = '\0';
    	return buffer ;
}

/**********************************/

int receiveint(int socketFD ) {
	int network_int ;
	int receivedBytes = recv(socketFD , &network_int , sizeof(int) , 0);
        if( receivedBytes == -1 ){
        	fprintf( stderr, "(CLIENT) Echec de reception du message du serveur\n" );
        	exit(1);
    	}
    	return ntohl(network_int);
} 

/**********************************/

int accueil(void){
    int n ;
    printf("\n\n****************************************\n\n"
           "Veuillez introduire votre choix : \n"
           "Tapez : 1 - pour se connecter .\n"
           "        2 - pour cree nouveau compte .\n"
           "        0 - pour quitter l'application .\n\n" );
    n = ecrire(0 , 2);
    printf("\n\n****************************************\n\n");
    return n ;
}

/**********************************/

int menu(void) {
    int n ;
    printf("\n\n******************-Menu de choix-******************\n\n"
           "Tapez : 1 - Boite de messagerie .\n"
           "        2 - Modification du compte .\n"
           "        3 - Suppression de Compte .\n"
           "        0 - Se Deconnecter . \n\n");
    n = ecrire ( 0 , 3 );
    return n ;
}

/**********************************/

char* c_user ( char user[] ){
    int i , b ;
    do{
        b = 0 ;
        printf("\nEntrez votre nom d'utilisateur (max 50 caracteres) : ");
        scanf("%s",user);
        if(strlen(user) > 50){
            printf("\nNom d'utilisateur trop long.\n");
        } 
        else if (strlen(user) == 0){
            printf("\nNom d'utilisateur vide .\n");
        }
        else {
            for (i = 0; (user)[i] != '\0'; i++) {
                if( isdigit(user[0]) || user[0] == '_' ){
                    printf("\nLe username ne doit pas commencer par des chiffres ou par '_' .\n");
                    b++;
                    break;
                }
                if (!isalnum(user[i]) && (user)[i] != '_'){
		        printf("\nLe username ne doit contenir que des lettres et des chiffres et des '_' .\n");
		        b++;
		        break;
                }
            }
        }
    }while(strlen(user) == 0 || strlen(user) > 50 || b );
    return user ;
}

/**********************************/

char* c_pass ( char  pass[] ) {
    do {
        printf("\nEntrez votre mot de passe (max 50 caracteres) : ");
        scanf("%s",pass);
        if (strlen(pass) > 50)
            printf("\nMot de passe trop long.\n");
        else if (strlen(pass) == 0)
            printf("\nMot de passe vide .\n");
        else if (strlen(pass) > 0 && strlen(pass) < 8)
            printf("\nMot de passe doit contenir au moins 8 caracteres .\n") ;
    }while(strlen(pass) > 50 || strlen(pass) == 0  || (strlen(pass) > 0 && strlen(pass) < 8));
    return pass ;
}

/**********************************/

void cree_c(int socketFD ) {
	int b  ;
	compte c ;
	char ligne[101] ;
	do {
		c_user(c.username); 
		sendint(socketFD , 0 );
 		sendmessage(socketFD , c.username);
 		b = receiveint(socketFD ) ;
 		if ( b == 0 ){
 			printf("Le nom d'utilisateur est deja pris.\n");
 		}
 	}while( b == 0 ) ; 
 	c_pass(c.password);
 	printf("\nTapez : 1 - Pour confirmer votre creation du compte .\n"
               "        0 - Pour annuler la creation du compte .\n\n") ;
        b = reessayer();
        sendint(socketFD , b );
        if ( b ) {
		user_pass(c.username , c.password , ligne);
           	sendmessage(socketFD , ligne ) ;
           	printf("\nVous avez cree votre compte avec succes.\n");
        }
        else
        	printf("\nLe processus de creation du compte a ete supprime.\n");
}

/**********************************/

int connecter_c ( int socketFD ) {
	int b  , t  ;
	char ligne[101] ;
	compte c ;
	do{
		c_user(c.username);
		c_pass(c.password);
		user_pass(c.username , c.password , ligne);
		sendmessage(socketFD , ligne);
		b = receiveint(socketFD);
		if ( b == 1 ) {
			printf("\nConnexion reussie pour l'utilisateur : %s\n", c.username);
			return 1 ;
		}
		else {
		    printf("\nNom d'utilisateur ou mot de passe incorrect. Connexion echouee.\n\n");
		    printf("Tapez : 1 - pour reessayer .\n"
			   "        0 - pour quitter .\n\n");
		    t = reessayer();
		    sendint(socketFD , t);
		}	
	}while(t);
	return 0 ;	
}

/**********************************/

int modification_c ( int socketFD ) {
	int n , b ;
	char ligne[101] = "\0" ;
	compte c , d ;
	printf("\n\n****************************************\n\n"
               "Veuillez introduire votre choix : \n"
               "Tapez : 1 - pour modifier votre nom d'utilisateur .\n"
               "        2 - pour changer votre mot de passe .\n"
               "        0 - pour quitter .\n\n" );
    	n = ecrire(0 , 2);
    	sendint(socketFD , n) ;
    	if ( n == 0 ) 
    		return 0 ;
    	do {
    		printf("\nVerification du compte : \n\n");
		c_user(c.username);
		c_pass(c.password);
	    	user_pass(c.username , c.password , ligne);
	    	sendmessage(socketFD , ligne);
	    	b = receiveint(socketFD);
	    	if ( b == 1 ) {
	    		printf("\n\n****************************************\n\n"
	    		       "\nNom d'utilisateur ou mot de passe incorrect. Connexion echouee.\n");
	    		printf("\nTapez : 1 - pour reessayer .\n"
                               "        0 - pour quitter .\n\n");
                        b = reessayer() ;
                        sendint(socketFD , b ) ; 
                        if ( b == 0 )
                        	return 0 ;
	    	}
    	}while(b);
    	if ( n == 1 ) {
    		do{	
    			b = 0 ; 
    			printf("\nNouveau nom d'utilisateur : \n");
    			c_user(d.username);
    			sendmessage(socketFD , d.username);
    			if (receiveint(socketFD)){
    				printf("\nLe nom d'utilisateur est deja pris.\n\n");
				printf("Tapez : 1 - pour reessayer .\n"
			  	       "        0 - pour quitter .\n\n");
			  	b = reessayer() ;
			  	sendint(socketFD , b );
			  	if ( b == 0 ) 
			  		return 0 ;
							  	
    			}
    		}while(b);
    		printf("\nTapez : 1 - Pour confirmer la modification du votre nom d'utilisateur .\n"
                       "        0 - Pour annuler la modification du votre nom d'utilisateur .\n\n") ;
                b = reessayer() ;
                sendint(socketFD , b );
                if ( b == 1 ) {
                	printf("\nModifications du compte enregistrees avec succes.\n");
                	sendmessage(socketFD , c.password);
                }
                else
                	printf("\nL'annulation de la modification du compte\n\n");
    	} 
    	if ( n == 2 ) {
    		printf("\nNouveau mot de passe : \n");
    		c_pass(d.password);
    		printf("\nTapez : 1 - Pour confirmer la modification du votre mot de passe .\n"
		       "        0 - Pour annuler la modification du votre mot de passe .\n\n") ;
		b = reessayer();
    		sendint(socketFD , b );
    		if ( b == 1 ) {
    			printf("\nModifications du compte enregistrees avec succes.\n");
    			user_pass(c.username , d.password , ligne ) ;
    			sendmessage(socketFD , ligne);
    		}
    		else
    			printf("\nL'annulation de la modification du compte\n\n");
    	}
	printf("\n\n****************************************\n\n");
	return 0 ;
}

/**********************************/

int supprimer_c(int socketFD ){
	int b ;
	compte c ;
	char ligne[101] = "\0" ;
	do {
    		printf("\nVerification du compte : \n\n");
		c_user(c.username);
		c_pass(c.password);
	    	user_pass(c.username , c.password , ligne);
	    	sendmessage(socketFD , ligne);
	    	b = receiveint(socketFD);
	    	if ( b == 1 ) {
	    		printf("\n\n****************************************\n\n"
	    		       "\nNom d'utilisateur ou mot de passe incorrect. Connexion echouee.\n");
	    		printf("\nTapez : 1 - pour reessayer .\n"
                               "        0 - pour quitter .\n\n");
                        b = reessayer() ;
                        sendint(socketFD , b ) ; 
                        if ( b == 0 )
                        	return 0 ;
	    	}
    	}while(b);
    	printf("\nTapez : 1 - Pour confirmer la suppression du compte .\n"
	       "        0 - Pour annuler la suppression du compte .\n\n") ;
	b = reessayer();
	sendint(socketFD , b ) ;
	if ( b ) {
		printf("\nLa suppression du compte a été effectuée avec succès.\n\n");
		return b ;
	}
	else {
		printf("\nL'annulation de la suppression du compte\n\n");
		return 0 ;
	}
	return 0 ;
}

/**********************************/

int ecrire( int d , int f ) {
	int n = 0;
	char buffer[100] = "\0" ;
	int b = 0;
	do {
		b = 0;
		if ( scanf("%d", &n) != 1 || n > f || n < d ){
            		printf("Erreur , il faut taper un entier entre %d et %d.\n",d,f);
            		fgets(buffer, sizeof(buffer), stdin);
            		printf("Tapez une autre fois : ");
            		b = 1 ;
        	}
        }while(b);
        getchar();
        return n ;
}

/**********************************/

int reessayer(){
    int n ;
    char buffer[100] = "\0" ;
    int b ;
    do{
    	b = 0 ;
        if (scanf("%d", &n) != 1 || (n != 1 && n != 0)) {
            printf("Erreur , il faut taper 1 ou 0.\n");
            fgets(buffer, sizeof(buffer), stdin);
            printf("Tapez une autre fois : ");
            b = 1 ;
        }
    }while(b);
    getchar();
    return n ;
}

/**********************************/

char* user_pass (char username[] , char password[] , char ligne[] ) {
	strcpy(ligne , username);
	strcat(ligne , ":");
	strcat(ligne , password);
	return ligne ;
}









































