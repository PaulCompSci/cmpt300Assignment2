#include <stdio.h>
#include <pthread.h>
# include <stdlib.h>
#include "list.h"

extern List *messageList; // Assume this is initialized in your main.c
extern pthread_mutex_t listMutex; // Ensure this is initialized in main.c

void* screen_output_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&listMutex);
        // Assuming List_first moves to the first item and returns it
        char *message = List_first(messageList); 
        if (message) {
            printf("%s", message);
            free(message); // Assuming List_first or List_remove frees the node but not the item
            List_remove(messageList); // Remove the message from the list after printing
        }
        pthread_mutex_unlock(&listMutex);
    }
    return NULL;
}
