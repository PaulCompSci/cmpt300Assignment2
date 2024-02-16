#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "list.h"

extern List *sendQueue;
extern pthread_mutex_t sendQueueMutex;
extern int udpSendSockfd; // This should be initialized and bound in your main.c
extern struct sockaddr_in remoteAddr; // This should be set up in your main.c
extern volatile int running; // Global flag to control the running of threads

void* udp_send_thread(void *arg) {
    while (running) {
        char *message = NULL;

        // Lock the mutex before accessing the send queue
        pthread_mutex_lock(&sendQueueMutex);
        if (List_count(sendQueue) > 0) {
            message = List_trim(sendQueue); // Assuming List_trim gets and removes the last item
        }
        pthread_mutex_unlock(&sendQueueMutex);

        if (message) {
            // Check for the termination signal
            if (strcmp(message, "!") == 0) {
                free(message);
                running = 0; // Update the running flag to signal other threads to stop
                break; // Exit the loop to terminate the thread
            }
            
            // Send the message using the UDP socket
            if (sendto(udpSendSockfd, message, strlen(message), 0,
                       (struct sockaddr *)&remoteAddr, sizeof(remoteAddr)) < 0) {
                perror("sendto failed");
            }
            free(message); // Free the message after sending
        }

        // Optionally sleep to prevent this loop from consuming too much CPU
        usleep(10000); // 10 ms
    }

    // Could send a termination message to the receiver if needed
    // sendto(..., "!", 1, ...);

    return NULL;
}
