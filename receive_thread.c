#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "list.h"

extern List *receiveQueue;
extern pthread_mutex_t receiveQueueMutex;
extern volatile int running; // Global flag to control the running of threads

void *screen_output_thread(void *arg) {
    while (running) {
        char *message = NULL;

        pthread_mutex_lock(&receiveQueueMutex);
        if (List_count(receiveQueue) > 0) {
            message = List_trim(receiveQueue); // Assuming List_trim removes from the end
        }
        pthread_mutex_unlock(&receiveQueueMutex);

        if (message) {
            if (strcmp(message, "!") == 0) {
                free(message);
                running = 0; // Set running flag to 0 to signal all threads to stop
                break; // Break the loop to terminate this thread
            }
            printf("%s\n", message);
            fflush(stdout); // Ensure the message is printed immediately
            free(message); // Free the message after printing
        }
    }
    return NULL;
}
