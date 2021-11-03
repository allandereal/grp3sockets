//
// Created by simon on 10/27/21.
//
// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "arithmetic.c"

#define PORT     8080
#define MAXLINE  1024
#define SIZE  1024

int len;

typedef struct {
    int length;
    char data[SIZE];
} Message;

typedef enum {
    OK, BAD, WRONG_LENGTH
} Status;

typedef struct sockaddr_in SocketAddress;

Status UDPsend(int socketNumber, Message *m, SocketAddress *destination) {
    int sentResult = sendto(socketNumber, m->data, m->length,
                            MSG_CONFIRM, (struct sockaddr *) destination,
                            sizeof(SocketAddress));
    if (sentResult < 0) {
        return WRONG_LENGTH;
    }
    return OK;
}

Status UDPreceive(int socketNumber, Message *fromClient, SocketAddress *origin) {
    int status = recvfrom(socketNumber, fromClient->data, SIZE,
                          MSG_WAITALL, (struct sockaddr *) origin,
                          &len);
    if (status < 0) {
        return WRONG_LENGTH;
    }
    return OK;
}

Status GetRequest(Message *callMessage, int serverSocket, SocketAddress *clientSocketAddress) {
    Status receiveStatus = UDPreceive(serverSocket, callMessage, clientSocketAddress);
    return receiveStatus;
}

Status SendReply(Message *replyMessage, int socketNumber, SocketAddress *clientSocketAddress) {
    Status sendStatus = UDPsend(socketNumber, replyMessage, clientSocketAddress);
    return sendStatus;
}

/* make a socket address using any of the addressses of this computer
   for a local socket on given port */
void makeReceiverSA(SocketAddress *sa, int port)
{
  sa->sin_family = AF_INET;
  sa->sin_port = htons(port);
  sa->sin_addr.s_addr = htonl(INADDR_ANY);
}

int main(int argc, char *argv[]) {
    int serverSocketNumber;
    char userInput[MAXLINE];
    SocketAddress serverSocketAddress, clientSocketAddress;
    Message fromClient;
    Message toClient;

    // Creating socket file descriptor
    if ((serverSocketNumber = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serverSocketAddress, 0, sizeof(serverSocketAddress));
    memset(&clientSocketAddress, 0, sizeof(clientSocketAddress));

    makeReceiverSA(&serverSocketAddress, PORT);
    if( bind(serverSocketNumber, (struct sockaddr *)&serverSocketAddress, sizeof(struct sockaddr_in))!= 0){
        perror("Bind failed\n");
        close(serverSocketNumber);
        exit(EXIT_FAILURE);
    }

    for (;;) {
        memset(userInput, 0, sizeof(userInput));
        memset(fromClient.data, 0, SIZE);
        memset(toClient.data, 0, SIZE);

        Status status = GetRequest(&fromClient, serverSocketNumber, &clientSocketAddress);
        if(strcmp(fromClient.data, "q") == 0){
            strcpy(toClient.data, "Server terminated.\n");
            toClient.length = SIZE;
            SendReply(&toClient, serverSocketNumber, &clientSocketAddress);
            printf("%s", toClient.data);
            close(serverSocketNumber);
            break;
        }

        //uncomment the line below to test the timeout feature
        //sleep(5); 
        if (status == OK) {
            printf("Client : %s\n", fromClient.data);
            if (isExpression(fromClient.data) == 1) {
                int result = evaluateExpression(fromClient.data);
                sprintf(toClient.data, "%d", result);
                toClient.length = SIZE;

            } else {
                if (strcmp(fromClient.data, "Stop") == 0) {
                    strcpy(toClient.data, "Ok");
                    toClient.length = SIZE;
                    SendReply(&toClient, serverSocketNumber, &clientSocketAddress);
                    break;
                } else if (strcmp(fromClient.data, "Ping") == 0) {
                    strcpy(toClient.data, "Ok");
                    toClient.length = SIZE;
                }

            }
            SendReply(&toClient, serverSocketNumber, &clientSocketAddress);
        }
    }


    close(serverSocketNumber);
    return 0;
}