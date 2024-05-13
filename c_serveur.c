#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "serveur.h"

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

void cree_s ( int connectedSocketFD ) {
	int b ;
	char ligne[101] ;
	compte c ;
	do {
		b = receiveint(connectedSocketFD ) ;
                receivemessage(connectedSocketFD, c.username);
                if (verifier_user(c.username , -1 ) == 0) {
                    	sendint(connectedSocketFD, 1);
                    	b = 1 ; } 
                else 
                    	sendint(connectedSocketFD, 0);
        } while (b == 0);
	if ( (b = receiveint(connectedSocketFD)) ) {
		receivemessage(connectedSocketFD , ligne ) ;
		FILE *fic = fopen("compte.txt" , "a");
        	fprintf(fic, "%s\n", ligne );
       		fclose(fic);
       		char fichier[50] = "\0" ;
       		fichier_c(ligne , fichier);
	   	fic = fopen(fichier , "w");
	   	fclose(fic);
	 }
}

/**********************************/

int connecter_s(int connectedSocketFD , int *l ) {
	int b  = 1 , t  ;
	char ligne[101] , test[101] ; 
	*l = -1 ;
	do{
		receivemessage(connectedSocketFD , test);
		FILE *fic = fopen("compte.txt" , "r" );
		while (fscanf(fic, "%s", ligne) == 1 && b != 0 ){
	        	b = strcmp(test , ligne ) ;
	        	(*l)++;
		}
		fclose(fic);
		if (b == 0) {
			sendint(connectedSocketFD , 1 );
			return 1 ;
		}
		else if ( b != 0 ){
			sendint(connectedSocketFD , 0 );
			*l = -1 ;
			t = receiveint(connectedSocketFD);
		}
	}while(t);
	return 0 ;
	
}

/**********************************/

int modification_s(int connectedSocketFD , int l ){
	char ligne[101] = "\0" ; 
	compte c ; 
	int n  , b ;
	n = receiveint(connectedSocketFD);
	if ( n == 0 ) 
		return 0 ;
	do {
		b = 0 ;
		receivemessage( connectedSocketFD , ligne ) ;
		if(verifier_compte(ligne , l)) 
			sendint(connectedSocketFD , 0);
		else {
			sendint(connectedSocketFD , 1); 
			b = receiveint(connectedSocketFD);
			if ( b == 0 )
				return 0 ; 
		}
	}while(b); 
	if ( n == 1 ) {
		do{
			b = 0 ;
			receivemessage(connectedSocketFD , c.username);
			if ( verifier_user(c.username , l ) ) {
				sendint(connectedSocketFD , 1 );
				b = receiveint(connectedSocketFD ) ;
				if ( b == 0 )
					return 0;
			}
			else
				sendint(connectedSocketFD , 0 );
		}while(b);
		b = receiveint(connectedSocketFD ) ;
		if ( b == 1 ){
			modifier_user_conversation(c.username , l );
			receivemessage(connectedSocketFD , c.password);
			user_pass(c.username , c.password , ligne );
			modifier_compte(ligne , l ) ;
		}
		else 
			return 0 ;
	}
	if ( n == 2 ) {
		b = receiveint(connectedSocketFD ) ;
		if ( b == 1 ){
			receivemessage(connectedSocketFD , ligne);
			modifier_compte(ligne , l ) ;
		}
		else 
			return 0 ;
	}
	return 0 ;
}

/**********************************/

void modifier_compte(char n_ligne[] , int l ) {
	char ligne[101] = "\0" ;
	int i = -1 ;
	FILE *fic = fopen("compte.txt" , "r" );
	FILE *temp = fopen("temp.txt" , "a" );
	while( fscanf(fic , "%s" , ligne ) == 1 ){
		if ( ++i == l ) 
			fprintf(temp , "%s\n" , n_ligne );
		else 
			fprintf(temp , "%s\n" , ligne );
	}
	fclose(fic);
	fclose(temp);
	remove("compte.txt");
       	rename("temp.txt" , "compte.txt");
}

/**********************************/

void modifier_user_conversation( char user_n[] , int l ) {
	char ligne[50] = "\0" ; 
	char user_a[50] = "\0" ;
	char fichier[70] = "\0" , fichier_n[70] = "\0"  ;
	user_des(user_a , l );
	user_conversation(user_a , fichier ) ;
	FILE *fic = fopen( fichier , "r" ) ;
	while ( fscanf(fic , "%s" , ligne ) == 1 ) 
		modifier_user_destinataire(ligne , user_a , user_n ); 
	fclose(fic);
	user_conversation(user_n , fichier_n ) ;
	rename( fichier , fichier_n ) ;
}

/**********************************/

void modifier_user_destinataire( char ligne[] , char user_a[] , char user_n[] ) {
	char test[50] = "\0" ;
	char fic1[101] = "\0" , fic2[101] = "\0" ;
	char fic3[101] = "\0" , fic4[101] = "\0" ;
	char fichier[70] = "\0" ;
	user_conversation(ligne , fichier) ;
	FILE *fich = fopen(fichier , "r" ); 
	FILE *temp = fopen("temp.txt" , "a" );
	while(fscanf(fich , "%s" , test ) == 1) {
		if ( strcmp(test , user_a) == 0 )
			fprintf(temp , "%s\n" , user_n) ;
		else 
			fprintf(temp , "%s\n" , test ) ;
	}
	fclose(temp);
	fclose(fich);
	remove(fichier);
	rename("temp.txt" , fichier );
	user_destinataire( fic1 ,  user_a , ligne );
	user_destinataire( fic2 ,  ligne  , user_a );
	user_destinataire( fic3 ,  user_n , ligne );
	user_destinataire( fic4 ,  ligne  , user_n);
	rename(fic1 , fic3);
	rename(fic2 , fic4);
}

/**********************************/

int supprimer_s( int connectedSocketFD , int l ) {
	int b ;
	char ligne[101] = "\0" ;
	do {
		b = 0 ;
		receivemessage( connectedSocketFD , ligne ) ;
		if(verifier_compte(ligne , l)) 
			sendint(connectedSocketFD , 0);
		else {
			sendint(connectedSocketFD , 1); 
			b = receiveint(connectedSocketFD);
			if ( b == 0 )
				return 0 ; 
		}
	}while(b); 
	b = receiveint(connectedSocketFD );
	if ( b ) {
		supprimer_user_conversation(l);
		supprimer_compte(l);
	}
	return 0 ;
}

/**********************************/

void supprimer_compte( int l ){
	char ligne[101] = "\0" ;
	int i = -1 ;
	FILE *fic = fopen("compte.txt" , "r" );
	FILE *temp = fopen("temp.txt" , "a" );
	while( fscanf(fic , "%s" , ligne ) == 1 ){
		if ( ++i != l ) 
			fprintf(temp , "%s\n" , ligne );
	}
	fclose(fic);
	fclose(temp);
	remove("compte.txt");
       	rename("temp.txt" , "compte.txt");
}

/**********************************/

void supprimer_user_conversation(int l){
	char ligne[50] = "\0" ; 
	char user_a[50] = "\0" ;
	char fichier[70] = "\0" ;
	char fichier_n[70] = "\0" ;
	char test1[101] = "\0" , test2[101] = "\0" ;
	user_des(user_a , l );
	user_conversation(user_a , fichier ) ;
	FILE *fic = fopen( fichier , "r" ) ;
	while ( fscanf(fic , "%s" , ligne ) == 1 ) {
		user_conversation(ligne , fichier_n);
		supprimer_user_destinataire(fichier_n , user_a );
		user_destinataire(test1 , user_a , ligne );
		remove(test1);
		user_destinataire(test2 , ligne , user_a );
		remove(test2);
	} 
	fclose(fic);
	remove(fichier);
}

/**********************************/

void supprimer_user_destinataire(char fichier[] , char username[] ){
	char ligne[50] = "\0" ; 
	FILE *fic = fopen( fichier , "r" ) ;
	FILE *temp = fopen ( "temp.txt" , "a" ) ;
	while ( fscanf(fic , "%s" , ligne ) == 1 ) {
		if ( strcmp(ligne , username) != 0 )
			fprintf(temp , "%s\n" , ligne ) ;
	}
	fclose(fic);
	fclose(temp);
	remove(fichier);
	rename("temp.txt" , fichier ) ;
}

/**********************************/

int verifier_user(char user[] , int l ){
    char ligne[100] = "\0" , test[50] = "\0"; 
    char *delimiter;
    int i = -1 ;
    FILE *fic = fopen("compte.txt" , "r");
    while(fscanf(fic, "%s", ligne) == 1){
        delimiter = strchr(ligne, ':');
        strncpy(test, ligne, delimiter - ligne);
        test[delimiter - ligne] = '\0';
        if(strcmp(test , user) == 0 && ++i != l)
            return 1 ;
    }
    fclose(fic);
    return 0 ;
}

/**********************************/

int verifier_compte(char test[] , int l){
    char ligne[100] = "\0" ;
    int i = -1 ;
    FILE *fic = fopen("compte.txt" , "r");
    while(fscanf(fic, "%s", ligne) == 1){
        if(++i == l && strcmp(test , ligne) == 0){
        	fclose(fic);
        	return 1 ;
        }
    }
    return 0 ;
}

/**********************************/

char* user_pass (char username[] , char password[] , char ligne[] ) {
	strcpy(ligne , username);
	strcat(ligne , ":");
	strcat(ligne , password);
	return ligne ;
}


/**********************************/












