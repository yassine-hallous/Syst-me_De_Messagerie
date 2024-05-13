#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include "serveur.h"

#define LISTENTING_PORT 5094
#define PENDING_QUEUE_MAXLENGTH 5
#define BUFFER_SIZE 1024


void *client_handler(void *arg) {
    int connectedSocketFD = *((int *)arg);
    int n, b, l = -1;

    while (1) {
        n = receiveint(connectedSocketFD);
        if (n == 1) {
            if (connecter_s(connectedSocketFD, &l)) {
                do {
                    b = receiveint(connectedSocketFD);
                    if (b == 1) {
                        menu_message_s(connectedSocketFD, l);
                    }
                    else if (b == 2) {
                        modification_s(connectedSocketFD, l);
                    }
                    else if (b == 3) {
                        if (supprimer_s(connectedSocketFD, l))
                            break;
                    }
                    else if (b == 0)
                        break;
                } while (b);
            }
        }
        else if (n == 2) {
            cree_s(connectedSocketFD);
        }
        else if (n == 0) {
            close(connectedSocketFD);
            pthread_exit(NULL);
        }
    }
}


int main() {
    
    	/**********************************/
    	
    	int socketFD = socket(AF_INET , SOCK_STREAM , 0);
    	if(socketFD == -1){
        	fprintf( stderr, "(SERVEUR) Echec d'initialisation de Socket\n" );
        	exit(1);
    	}

    	struct sockaddr_in socketAddress;
    	socketAddress.sin_family = AF_INET ;
    	socketAddress.sin_port = htons(LISTENTING_PORT);
    	socketAddress.sin_addr.s_addr = INADDR_ANY;

    	int socketAddresslenght = sizeof(socketAddress);
    	int bindReturncode = bind(socketFD , (struct sockaddr*) &socketAddress , socketAddresslenght);
    	if (bindReturncode == -1 ){
        	fprintf( stderr, "(SERVEUR) Echec de liaison de Socket\n" );
       		exit(1);
    	}

    	if(listen(socketFD , PENDING_QUEUE_MAXLENGTH) == -1 ){
        	fprintf( stderr, "(SERVEUR) Echec de demarrage de l'ecoute des connexions entrantes\n" );
        	exit(1);
    	}
    	puts("En attente de nouvelles connexions...");

    	while (1) {
		int connectedSocketFD = accept(socketFD, (struct sockaddr*) &socketAddress, (socklen_t *) &socketAddresslenght);
		if (connectedSocketFD == -1) {
			fprintf(stderr, "(SERVEUR) Echec d'établissement de la connexion\n");
			exit(1);
		}

		pthread_t thread_id;
		if (pthread_create(&thread_id, NULL, client_handler, (void *)&connectedSocketFD) < 0) {
		    perror("Impossible de créer le thread");
		    return 1;
		}

		pthread_detach(thread_id);
	    	}
	close(socketFD);
    	return 0;
}
