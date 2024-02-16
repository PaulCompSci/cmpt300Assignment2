#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h> 
#include "list.h"

extern List *receiveQueue;
extern pthread_mutex_t receiveQueueMutex;
extern int udpRecvSockfd; // Changed from udpReceiveSockfd to udpRecvSockfd

void* udp_receive_thread(void *arg) {
    char buffer[1024]; // Buffer for storing incoming messages

    while (1) {
        ssize_t messageLen = recvfrom(udpRecvSockfd, buffer, sizeof(buffer) - 1, 0, NULL, NULL);
        
        if (messageLen < 0) {
            perror("Error receiving message");
            continue; // Continue receiving other messages
        }
        
        buffer[messageLen] = '\0'; // Null-terminate the received message
        
        // Lock the mutex before adding the message to the queue
        pthread_mutex_lock(&receiveQueueMutex);
        List_append(receiveQueue, strdup(buffer)); 
        pthread_mutex_unlock(&receiveQueueMutex);
    }
    
    return NULL;
}
