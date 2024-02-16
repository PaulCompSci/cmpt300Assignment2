
// #include <stdio.h>
// #include <pthread.h>
// # include <stdlib.h>
// #include "list.h"

// extern List *messageList; // Assume this is initialized in your main.c
// extern pthread_mutex_t listMutex; // Ensure this is initialized in main.c

// void* screen_output_thread(void *arg) {
//     while (1) {
//         pthread_mutex_lock(&listMutex);
//         // Assuming List_first moves to the first item and returns it
//         char *message = List_first(messageList); 
//         if (message) {
//             printf("%s", message);
//             free(message); // Assuming List_first or List_remove frees the node but not the item
//             List_remove(messageList); // Remove the message from the list after printing
//         }
//         pthread_mutex_unlock(&listMutex);
//     }
//     return NULL;
// }
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
