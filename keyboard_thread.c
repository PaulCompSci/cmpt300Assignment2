#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "list.h"

extern List *sendQueue;
extern pthread_mutex_t sendQueueMutex;

void *keyboard_input_thread(void *arg) {
    char message[256]; // Assuming messages are less than 256 characters

    while (fgets(message, sizeof(message), stdin)) {
        // Removing newline character from message if present
        message[strcspn(message, "\n")] = 0;
        
        // Check for exit command, assuming '!' is the exit command
        if (strcmp(message, "!") == 0) {
            // Add an exit message to the queue to signal other threads to shutdown
            pthread_mutex_lock(&sendQueueMutex);
            List_append(sendQueue, strdup("!"));
            pthread_mutex_unlock(&sendQueueMutex);
            break;
        }

        // Lock the mutex before adding the message to the queue
        pthread_mutex_lock(&sendQueueMutex);
        List_append(sendQueue, strdup(message)); // strdup dynamically allocates memory for the message
        pthread_mutex_unlock(&sendQueueMutex);
    }

    return NULL;
}
