typedef enum {
    OK, BAD, WRONG_LENGTH, TIMEOUT
} Status;

typedef struct sockaddr_in SocketAddress;

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