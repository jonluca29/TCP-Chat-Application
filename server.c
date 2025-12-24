#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


struct acceptedSock{
    int acceptedSockFD;
    int acceptSuccess;
};

struct acceptedSock sockets[10];
int acceptedSockCount = 0;

pthread_mutex_t sockMutex = PTHREAD_MUTEX_INITIALIZER; 

void *handleClient(void *arg){
    int cliSock = *(int*) arg;
    free(arg);
    char msg[256];


    // event loop
    while (1){
        // Clear the buffer before receiving
        memset(msg, 0, sizeof(msg));
       
        // receive message
        int bytes_received = recv(cliSock, msg, sizeof(msg) - 1, 0);    // sizeof(msg)-1 to leave space for manual null termination
        
        // check for client disconnection or error
        if (bytes_received == 0) {
            printf("Client disconnected\n");
            break;
        }else{
            printf("Error occured.\n");
        }
        
        // Ensure null termination
        msg[bytes_received] = '\0';
       
        // check if the client wants to quit
        if (strcmp(msg, "QUIT") == 0){
            printf("Client sent QUIT. Closing connection.\n");
            break;
        }
       
        // print the client's message to the server terminal
        printf("Received: \"%s\"\n", msg);
       
        // send the client's message to the rest of the clients
        pthread_mutex_lock(&sockMutex);
        for (int i = 0; i < acceptedSockCount; i++){
            if (sockets[i].acceptedSockFD != cliSock){
                send(sockets[i].acceptedSockFD, msg, strlen(msg)+1, 0);
            }
        }
        pthread_mutex_unlock(&sockMutex);
        
    } 
    close(cliSock);
    return NULL;
}


int createSocket(){
    return socket(AF_INET, SOCK_STREAM, 0);
}

int specifyAddressAndBind(int sockfd, struct sockaddr_in* address){
    address->sin_family = AF_INET;
    address->sin_port = htons(9002);
    address->sin_addr.s_addr = INADDR_ANY;
    
    return bind(sockfd, (struct sockaddr*)address, sizeof(*address));
}

void acceptClients(int sockfd){
    while(1){
        // accept a connection for every client;
        int *cliSock = malloc(sizeof(int));
        *cliSock = accept(sockfd, NULL, NULL);
        if (*cliSock == -1){
            printf("Accept failed.\n");
            free(cliSock);
            continue;
        }
        
        pthread_mutex_lock(&sockMutex);
        sockets[acceptedSockCount++].acceptedSockFD = *cliSock;     // keep track of the accepted clients 
        pthread_mutex_unlock(&sockMutex);

        printf("New client connected\n");


        pthread_t threadID;
        pthread_create(&threadID, NULL, handleClient, cliSock);
        pthread_detach(threadID);
    }
}


int main(){
    
    // create the socket
    int sockfd = createSocket();
    if (sockfd == -1){
        printf("sock creation failed.\n");
        return 1;
    }


    // define the server address and bind the socket to the address
    struct sockaddr_in server_address;
    if (specifyAddressAndBind(sockfd, &server_address) == -1){
        printf("bind failed.\n");
        return 1;
    }

   
    // start listening
    if (listen(sockfd, 10) == -1){
        printf("Listening failed.\n");
        return 1;
    }

    // confirm listening
    printf("server lisentin on port 9002\n");


    // accept the clients
    acceptClients(sockfd);

    printf("shutting down.\n");

    // close the socket
    close(sockfd);
    
    return 0;
}
