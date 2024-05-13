#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include "serveur.h"

#define CONNECTION_HOST "127.0.0.1"
#define LISTENTING_PORT 5094
#define BUFFER_SIZE 1024

/**********************************/

void menu_message_s(int connectedSocketFD , int l ){
	FILE *fic ;
	int i = -1  , n  ;
	char ligne[101] = "\0" , fichier[70] = "\0" ;
	fic = fopen("compte.txt" , "r" );
	while( i < l ){
		i++;
		fscanf(fic , "%s" , ligne );
	}
	fclose(fic);
	fichier_c(ligne , fichier);
   	do{
   		fic = fopen( fichier , "r" ) ;
	   	fseek(fic, 0, SEEK_END);
	   	int taille_fichier = ftell(fic);
	   	fclose(fic);
	   	if ( taille_fichier == 0 ) {
	   		sendint(connectedSocketFD , 0 ) ;
	   		n = receiveint(connectedSocketFD);
	   		if (n){
	   			ajouter_s(connectedSocketFD , fichier , l ) ;
	   		}
	   		
	   	} 
	   	else {
	   		sendint(connectedSocketFD , 1);
			fic = fopen(fichier , "r");
		   	while(fscanf(fic, "%s", ligne) == 1){
		  		sendint(connectedSocketFD , 1 );
	   			sendmessage(connectedSocketFD , ligne);
	   			if ( receiveint(connectedSocketFD)){};
		   	}
			fclose(fic);
		   	sendint(connectedSocketFD , 0);
			n = receiveint(connectedSocketFD) ;
	  		if ( n == -1 ) {
		  		ajouter_s(connectedSocketFD , fichier , l ) ;
		  	}
		  	else if  ( n == -2 ) {
		  		supprimer_destinataire_s ( connectedSocketFD , fichier , l) ;
		  	}
		  	else if ( n != 0 ) {
		  		conversation_s( connectedSocketFD , fichier , n , l );
	   		}
	   	}
  	}while(n != 0);	
}

/**********************************/

int conversation_s(int connectedSocketFD , char fichier[] , int n , int l) {
	int i = 0 , b = 0 ;
	char destinataire[50] = "\0";
	char username[50] = "\0";
	char user_file[101] = "\0" , dest_file[101] = "\0" ; 
	char message[1000] = "\0" ;
	FILE *fic = fopen( fichier , "r" );
	while(i < n){
		i++;
		fscanf(fic , "%s" , destinataire);
	}
	fclose(fic);
	user_des(username , l );
	user_destinataire(user_file , username , destinataire);
	user_destinataire(dest_file , destinataire , username);
   	do {	
   		fic = fopen(user_file , "r");
		fseek(fic , 0 , SEEK_END);
		int taille_fichier = ftell(fic);
	   	fclose(fic);
   		if ( taille_fichier == 0 ) {
   			sendint(connectedSocketFD , 0 ) ;
   			b = receiveint(connectedSocketFD ) ;
   			if ( b ) {
   				envoyer_s(connectedSocketFD , user_file , dest_file , username ); 
   			}
   			else 
   				return 0 ;
   		}
   		else {
   			sendint(connectedSocketFD , 1 );
   			sendmessage(connectedSocketFD , destinataire);
   			receiveint(connectedSocketFD);
   			fic = fopen(user_file , "r" ) ;
   			while(fgets(message , 1000 , fic) != NULL ){
	   			sendint(connectedSocketFD , 1 );
	   			size_t len = strlen(message);
				if (len > 0 && message[len - 1] == '\n') {
					message[len - 1] = '\0';
				}
	   			sendmessage(connectedSocketFD , message);
	   			if ( receiveint(connectedSocketFD)){};
		   	}
			fclose(fic);
		   	sendint(connectedSocketFD , 0);
		   	b = receiveint(connectedSocketFD) ;
		   	if ( b == 1 ) {
		   		envoyer_s(connectedSocketFD , user_file , dest_file , username );
		   		sendint(connectedSocketFD , 1 ) ;
		   	}
		   	else if ( b == 2 ) {
		   		supprimer_message_s(connectedSocketFD , user_file , dest_file ) ;
		   	}
		   	else if ( b == 0 ){
		   		return 0 ;
		   	} 
   		}
   	}while(b);
	return 0 ;
}

/**********************************/

void envoyer_s( int connectedSocketFD , char user_f[] , char dest_f[] , char username[] ){
	char message[1000] ; 
	char time_string[50];
	time_s(time_string , sizeof(time_string) );
	receivemessage(connectedSocketFD , message);
	FILE *fic = fopen(user_f , "a" );
	FILE *temp = fopen(dest_f , "a");
	fprintf(fic , "%s [%s] : %s\n" , time_string , username , message ) ; 
	fprintf(temp , "%s [%s] : %s\n" , time_string , username , message ) ; 
	fclose(fic);
	fclose(temp);
}

/**********************************/

void supprimer_message_s ( int connectedSocketFD , char user_f[] , char dest_f[] ) {
	int n , b , i  ;
	char message[1000] ;
	n = receiveint(connectedSocketFD);
	b = receiveint(connectedSocketFD); 
	if ( b ) {
		i = 0 ;
		FILE *fic = fopen( user_f , "r" ) ;
		FILE *temp = fopen ( "temp.txt" , "a" ) ;
		while ( fgets(message , 1000 , fic) != NULL ) {
			i++;
			if ( i != n ){
				size_t len = strlen(message);
				if (len > 0 && message[len - 1] == '\n') {
					message[len - 1] = '\0';
				}
				fprintf(temp , "%s\n" , message ) ;
			}
		}
		fclose(fic);
		fclose(temp);
		remove(user_f);
		rename("temp.txt" , user_f) ;
		//
		i = 0 ;
		fic = fopen( dest_f , "r" ) ;
		temp = fopen ( "temp.txt" , "a" ) ;
		while ( fgets(message , 1000 , fic) != NULL ) {
			i++;
			if ( i != n ){
				size_t len = strlen(message);
				if (len > 0 && message[len - 1] == '\n') {
					message[len - 1] = '\0';
				}
				fprintf(temp , "%s\n" , message ) ;
			}
		}
		fclose(fic);
		fclose(temp);
		remove(dest_f);
		rename("temp.txt" , dest_f) ;
	}
}

/**********************************/

int ajouter_s(int connectedSocketFD , char fichier[] , int l ) {
	char username[50] = "\0" ;
	char test1[101] = "\0" , test2[101] = "\0" , test[101] = "\0" ;
	int b  ;
	char destinataire[50] = "\0" ;
	do {
		b = 0 ;
                receivemessage(connectedSocketFD, destinataire);
                if (verifier_destinataire(destinataire , fichier , l ) == 1) {
                    	sendint(connectedSocketFD, 1);
                } 
                else {
                    	sendint(connectedSocketFD, 0);
                    	b = receiveint(connectedSocketFD );
                    	if ( b == 0 ) 
                    		return 0 ;
                }
        } while (b == 1);
        FILE *fic = fopen(fichier, "a");
        fprintf(fic, "%s\n", destinataire );
       	fclose(fic);
       	user_des(username , l ) ;
       	user_conversation(destinataire , test);
       	fic = fopen(test , "a" );
       	fprintf(fic, "%s\n", username);
       	fclose(fic);
       	user_destinataire(test1 , username , destinataire );
       	user_destinataire(test2 , destinataire , username );
       	fic = fopen(test1 , "w" );
       	fclose(fic);
       	fic = fopen( test2 , "w" );
       	fclose(fic);
       	return 0 ;
}

/**********************************/

void supprimer_destinataire_s ( int connectedSocketFD , char fichier[] , int l ) {
	int n , b , i = 0 ;
	char ligne[100] = "\0" ;
	char username[50] = "\0" ;
	char destinataire[50] = "\0" ;
	char test1[101] = "\0" , test2[101] = "\0" , test[101] = "\0" ;
	n = receiveint(connectedSocketFD);
	b = receiveint(connectedSocketFD); 
	if ( b ) {
		FILE *fic = fopen( fichier , "r" ) ;
		FILE *temp = fopen ( "temp.txt" , "a" ) ;
		while ( fscanf(fic , "%s" , ligne ) == 1 ) {
			i++;
			if ( i != n )
				fprintf(temp , "%s\n" , ligne ) ;
			else
				strcat(destinataire , ligne ) ; 
		}
		fclose(fic);
		fclose(temp);
		remove(fichier);
		rename("temp.txt" , fichier ) ;
		user_conversation(destinataire , test);
		supprimer_user_destinataire(test , username);
		user_des(username , l);
		user_destinataire(test1 , username , destinataire );
	       	remove(test1);
	       	user_destinataire(test2 , destinataire , username );
	       	remove(test2);
	}
	
}

/**********************************/

int verifier_destinataire(char destinataire[] , char fichier[] , int l ){
    char ligne[100] , test[50]  ; 
    char *delimiter;
    int i = -1 ; 
    FILE *fic = fopen("compte.txt" , "r");
    while(fscanf(fic, "%s", ligne) == 1){
    	i++;
        delimiter = strchr(ligne, ':');
        strncpy(test , ligne , delimiter - ligne);
        test[delimiter - ligne] = '\0';
        if(strcmp(test , destinataire) == 0 && i != l){
        	fclose(fic);
        	fic = fopen(fichier , "r");
		while(fscanf(fic, "%s", ligne) == 1){
			if(strcmp(destinataire , ligne) == 0 ){
				fclose(fic);
				return 0 ;
			}
		}
		fclose(fic);
		return 1 ;
        }    
    }
    fclose(fic);
    return 0 ;
}

/**********************************/

char* fichier_c(char ligne[] , char fichier[] ) {
	char *delimiter ;
    	delimiter = strchr(ligne, ':');
   	strncpy(fichier , ligne , delimiter - ligne);
   	strcat(fichier , "_conversation" );
   	return fichier;
}

/**********************************/

char* user_conversation(char username[] , char fichier[]){
	strcpy(fichier , username);
	strcat(fichier , "_conversation");
	return fichier;
}

/**********************************/
 
char* user_des(char username[] , int l ){
	int i = -1;
	char *delimiter;
	char ligne[100];
	FILE *fic = fopen("compte.txt" , "r");
    	while(fscanf(fic, "%s", ligne) == 1 && i != l){
    		if ( ++i == l ){
			delimiter = strchr(ligne, ':');
			strncpy(username , ligne , delimiter - ligne);
			username[delimiter - ligne] = '\0';
		}
	}
	fclose(fic);
	return username;
}

/**********************************/

char* user_destinataire( char test[] ,  char username[] , char destinataire[] ) {
	strcpy( test , username );
	strcat( test , "_" ) ;
	strcat (test , destinataire ) ;
	return test ;
} 

/**********************************/

char* time_s(char time_string[] , size_t size) {
	time_t current_time;
    	struct tm *time_info;
    	time(&current_time);
    	time_info = localtime(&current_time);
    	strftime(time_string, size, "[%Y-%m-%d]  [%H:%M:%S]", time_info);
    	return time_string ;
}



