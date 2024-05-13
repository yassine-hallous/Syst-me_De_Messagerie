#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "client.h"

#define CONNECTION_HOST "127.0.0.1"
#define LISTENTING_PORT 5094
#define BUFFER_SIZE 1024


int main() {
	
	/**********************************/
    	
    	int socketFD = socket(AF_INET , SOCK_STREAM , 0);
    	if(socketFD == -1){
        	fprintf( stderr, "(CLIENT) Echec d'initialisation de Socket\n" );
        	exit(1);
    	}

    	struct sockaddr_in socketAddress;
    	socketAddress.sin_family = AF_INET ;
    	socketAddress.sin_port = htons(LISTENTING_PORT);

   	int inetReturncode = inet_pton(AF_INET , CONNECTION_HOST , &socketAddress.sin_addr);
  	if ( inetReturncode == -1 ){
        	fprintf( stderr, "(CLIENT) Adress invalide ou non prise en charge\n" );
        	exit(1);
    	}
    	
    	int socketAddresslenght = sizeof(socketAddress);
    		int connectStatus = connect(socketFD , (struct sockaddr*) &socketAddress , socketAddresslenght);
    		if (connectStatus == -1 ){
        		fprintf( stderr, "(CLIENT) Echec de la connexion au serveur\n" );
      		  	exit(1);
  		}
  		 	
  	/**********************************/
    	 	
	int n ;
    	while(1) {
		n = accueil() ;
		sendint(socketFD , n ) ;
		if ( n == 1 ){
			if ( connecter_c(socketFD) ){
				do{
                    			n = menu();
                    			sendint(socketFD , n ) ;
                    			if ( n == 1 ) {
                    				menu_message_c(socketFD) ;
                    			}
                    			if ( n == 2 ) {
                       				modification_c(socketFD); 
                    			}
                    			else if ( n == 3 ) {
                        			if( supprimer_c(socketFD ))
                            				break;
                    			}
                    			else if ( n == 0 ) {
                    				break;
                    			}
                		}while(n);
                	}
		}   	
		else if ( n == 2 ) {
			cree_c(socketFD);		 			
		}
		else if ( n == 0 ) {
			close(socketFD);
    			return 0 ;
		}
    	}	
      
}
