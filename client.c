//
// Created by simon on 10/27/21.
//
// Client side implementation of UDP client-server model
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

#define PORT     8080
#define MAXLINE  1024
#define SIZE  1024
#define TIMEOUT_SEC 5
#define RETRY 3

typedef struct {
    int length;
    char data[SIZE];
} Message;

typedef enum {
    OK, BAD, WRONG_LENGTH, TIMEOUT
} Status;

typedef struct sockaddr_in SocketAddress;

#include <sys/time.h>
/* use select to test whether there is any input on descriptor s*/
int anyThingThere(int s)
{
  unsigned long read_mask;
  struct timeval timeout;
  int n;
  
  timeout.tv_sec = TIMEOUT_SEC; /*seconds wait*/
  timeout.tv_usec = 0; /* micro seconds*/
  read_mask = (1<<s);
  if((n = select(32, (fd_set *)&read_mask, 0, 0, &timeout))<0)
    perror("Select fail:\n");
  //else printf("n = %d\n");
  return n;
}

Status UDPsend(int socketNumber, Message *m, SocketAddress destination) {
    int sentResult = sendto(socketNumber, m->data, m->length,
                            MSG_CONFIRM, (struct sockaddr *) &destination,
                            sizeof(destination));
    if (sentResult < 0) {
        return WRONG_LENGTH;
    }
    return OK;
}

Status UDPreceive(int socketNumber, Message *m, SocketAddress *origin) {
    int len = sizeof(origin);
    int status = recvfrom(socketNumber, m->data, m->length,
                          MSG_WAITALL, (struct sockaddr *) &origin,
                          &len);
    if (status < 0) {
        return WRONG_LENGTH;
    }
    return OK;
}

Status DoOperation(Message *message, Message *reply, int socketNumber, SocketAddress serverSocketAddress) {
    int n;
    Status sendStatus = UDPsend(socketNumber, message, serverSocketAddress);

    n = anyThingThere(socketNumber);
    if(n==0){
        return TIMEOUT;
    }

    Status receiveStatus = UDPreceive(socketNumber, reply, &serverSocketAddress);

    if (sendStatus != WRONG_LENGTH && receiveStatus != WRONG_LENGTH) {
        return OK;
    }
    return WRONG_LENGTH;
}

/* make a socket address using any of the addressses of this computer
   for a local socket on any port */
void makeLocalSA(SocketAddress *sa)
{
  sa->sin_family = AF_INET;
  sa->sin_port = htons(0);
  sa->sin_addr.s_addr = htonl(INADDR_ANY);
}

/* make a socket address for a destination whose machine and port
   are given as arguments */
void makeDestSA(SocketAddress *sa, char *hostname, int port)
{
  struct hostent *host;
  
  sa->sin_family = AF_INET;
  if((host = gethostbyname(hostname))== NULL) {
    printf("Unknown host name (%s)\n", hostname);
    exit(-1);
  }
  sa->sin_addr = *(struct in_addr *) (host->h_addr);
  sa->sin_port = htons(port);
}


int main(int argc, char *argv[]) {
    int clientSocketNumber;
    int retries = 0;
    char userInput[SIZE];
    SocketAddress clientSocketAddress, serverSocketAddress;
    Message message;
    Message reply;

    // Creating client socket file descriptor
    if ((clientSocketNumber = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    makeLocalSA(&clientSocketAddress);
    if( bind(clientSocketNumber, (struct sockaddr *)&clientSocketAddress, sizeof(struct sockaddr_in))!= 0){
        perror("Bind failed\n");
        close (clientSocketNumber);
        return 0;
    }

    memset(&serverSocketAddress, 0, sizeof(serverSocketAddress));

    // Create Server Socket
    makeDestSA(&serverSocketAddress, argv[1], PORT);

    memset(userInput, 0, sizeof(userInput));

    memset(message.data, 0, SIZE);
    memset(reply.data, 0, SIZE);
    reply.length = SIZE;
    Status status;

    for(;;) {
        printf("Enter Text: ");
        scanf("%s", userInput);

        if(strcmp(userInput, "q") == 0) break;

        strcpy(message.data, userInput);
        message.length = strlen(message.data);

        DoOperation: status = DoOperation(&message, &reply, clientSocketNumber, serverSocketAddress);
        if (status == OK) {
            retries = 0;
            printf("Server: %s\n", reply.data);
        } else if(status == TIMEOUT)  {
            if(retries <= RETRY){
                printf("Retrying Operation...\n");
                retries++;
                goto DoOperation;
            } else {
                printf("The Server timed out!\n");
                close(clientSocketNumber);
                break;
            }
        } else {
            printf("Internal Server Error!\n");
            close(clientSocketNumber);
            break;
        }

        memset(userInput, 0, sizeof(userInput));

        memset(message.data, 0, SIZE);
        memset(reply.data, 0, SIZE);
    }


    close(clientSocketNumber);
    return 0;
}
