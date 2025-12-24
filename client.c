#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

void *listenForMsgs(void *arg){
    int sockfd = *(int*) arg;
    char res[256];

    // event loop
    while (1){
        // clear before receiving  
        memset(res, 0, sizeof(res));
        
        int bytesRecv = recv(sockfd, res, sizeof(res)-1, 0);
        if (bytesRecv <= 0){
            printf("Client disconnected or error occured.");
            break;
        }

        // Ensure null termination
        res[bytesRecv] = '\0';

        // print received message
        printf("%s\n", res);
    }
    return NULL;
}

int createSocket(){
    return socket(AF_INET, SOCK_STREAM, 0);
}

int specifyAddressAndConnect(int sockfd, struct sockaddr_in* address){ 
    address->sin_family = AF_INET;
    address->sin_port = htons(9002);
    address->sin_addr.s_addr = INADDR_ANY;   
    
    return connect(sockfd, (struct sockaddr *)address, sizeof(*address));
}

void sendMessages(int sockfd){
    char msg[256];      // the full message containing the sender name and message content 
    char content[246];  // the  content of the message
    char *name = NULL;
    size_t nameSize = 0;
    
    printf("Please enter a name: ");
    ssize_t nameCount = getline(&name, &nameSize, stdin);
    name[nameCount-1] = '\0';

    printf("Type and press enter to send a message (QUIT to exit)\n");

    // message loop
    // user will send messages until they decide to quit
    while (1){
        if (fgets(content, 245, stdin) == NULL){
            break;
        }
        
        // ensure null termination
        size_t len = strlen(content);
        if (len > 0 && content[len-1] == '\n'){
            content[len-1] = '\0';
        }
               
        if (strcmp(content, "QUIT") == 0){
            printf("Close connection.\n"); 
            break;
        }
       
        sprintf(msg, "%s: %s", name, content);

        send(sockfd, msg, strlen(msg)+1, 0);
    }
}

int main(){    
    int sockfd = createSocket();
    if (sockfd == -1){
        printf("Socket creation failed.\n");
        return 1;
    }

    // specify the address for the socket and connect to the server
    struct sockaddr_in server_address;
    int connectStatus = specifyAddressAndConnect(sockfd, &server_address);
    
    // error handle for connection errors
    if (connectStatus == -1){
        printf("Error while connecting\n");
        close(sockfd);
        return 1;
    }
    
    
    printf("connected to server.\n");


    // create a thread to receive  messages
    pthread_t threadID;
    pthread_create(&threadID, NULL, listenForMsgs, &sockfd);

    
    // the main thread will be used for sending messages
    sendMessages(sockfd);


    // print goodbyte message
    printf("Bye bye\n");
    
    close(sockfd);
    
    return 0;
}
