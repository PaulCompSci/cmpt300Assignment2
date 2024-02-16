#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // For string functions
#include <unistd.h>     // For usleep
#include <sys/socket.h> // For sendto and recvfrom
#include <netinet/in.h> // For struct sockaddr_in
#include "list.h"
# include <arpa/inet.h>
#include <sys/_endian.h>

// Global variables
volatile int running = 1;          // Control flag for running threads
pthread_mutex_t sendQueueMutex;    // Mutex for the send queue
pthread_mutex_t receiveQueueMutex; // Mutex for the receive queue
pthread_mutex_t listMutex;         // Mutex for the message list
List *sendQueue;                   // Queue for messages to be sent
List *receiveQueue;                // Queue for received messages
int udpSendSockfd;                 // Socket file descriptor for sending UDP messages
int udpRecvSockfd;                 // Socket file descriptor for receiving UDP messages
struct sockaddr_in remoteAddr;     // Remote address for sending messages

// Placeholder function declarations for thread routines
void *keyboard_input_thread(void *arg);
void *screen_output_thread(void *arg);
void *udp_send_thread(void *arg);
void *udp_receive_thread(void *arg);

// Global list accessible by all threads
List *messageList;

// Replace with the actual port number you want to use
#define PORT 5000

// Replace with the actual IP address or domain name of the server
#define IP_ADDRESS "127.0.0.1"

int main()
{

   memset(&remoteAddr, 0, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, IP_ADDRESS , &remoteAddr.sin_addr) <= 0) { 
        // Handle error
        return -1;
    }


    pthread_mutex_init(&sendQueueMutex, NULL);
    pthread_mutex_init(&receiveQueueMutex, NULL);
    pthread_mutex_init(&listMutex, NULL);
    pthread_t tid_keyboard, tid_screen, tid_udp_send, tid_udp_receive;

    printf("this is a good start!");
    // Initialize the list
    messageList = List_create();

    // Check for list creation failure
    if (!messageList)
    {
        // Handle error
        return -1;
    }

    // Create threads
    pthread_create(&tid_keyboard, NULL, keyboard_input_thread, NULL);
    pthread_create(&tid_screen, NULL, screen_output_thread, NULL);
    pthread_create(&tid_udp_send, NULL, udp_send_thread, NULL);
    pthread_create(&tid_udp_receive, NULL, udp_receive_thread, NULL);

    // Wait for threads to finish
    pthread_join(tid_keyboard, NULL);
    pthread_join(tid_screen, NULL);
    pthread_join(tid_udp_send, NULL);
    pthread_join(tid_udp_receive, NULL);

    // Cleanup
    // Free list and other resources
    pthread_mutex_destroy(&sendQueueMutex);
    pthread_mutex_destroy(&receiveQueueMutex);
    pthread_mutex_destroy(&listMutex);

    return 0;
}

void *keyboard_input_thread(void *arg)
{
    char message[256];
    while (fgets(message, sizeof(message), stdin) && running)
    {
        char *pos;
        if ((pos = strchr(message, '\n')) != NULL)
            *pos = '\0'; // Remove newline character

        pthread_mutex_lock(&sendQueueMutex);
        if (strcmp(message, "exit") == 0)
            running = 0;                         // Exit command
        List_append(sendQueue, strdup(message)); // Duplicate message to heap
        pthread_mutex_unlock(&sendQueueMutex);
    }
    return NULL;
}
void *screen_output_thread(void *arg)
{
    while (running)
    {
        pthread_mutex_lock(&listMutex);
        char *message = (char *)List_trim(messageList); // Assume List_trim returns last item
        pthread_mutex_unlock(&listMutex);

        if (message)
        {
            printf("%s\n", message);
            free(message); // Assume message was dynamically allocated
        }
        usleep(100000); // Sleep for 100ms to avoid busy waiting
    }
    return NULL;
}
void *udp_send_thread(void *arg)
{
    while (running)
    {
        pthread_mutex_lock(&sendQueueMutex);
        char *message = (char *)List_trim(sendQueue); // Get message to send
        pthread_mutex_unlock(&sendQueueMutex);

        if (message)
        {
            ssize_t bytes_sent = sendto(udpSendSockfd, message, strlen(message), 0,
                                        (struct sockaddr *)&remoteAddr, sizeof(remoteAddr));
            if (bytes_sent < 0)
            {
                // Handle error
            }
            free(message); // Cleanup
        }
        usleep(100000); // Sleep for 100ms
    }
    return NULL;
}
void *udp_receive_thread(void *arg)
{
    char buffer[1024];
    while (running)
    {
        ssize_t messageLen = recvfrom(udpRecvSockfd, buffer, sizeof(buffer) - 1, 0, NULL, NULL);
        if (messageLen > 0)
        {
            buffer[messageLen] = '\0'; // Null-terminate the received message
            pthread_mutex_lock(&receiveQueueMutex);
            List_append(receiveQueue, strdup(buffer)); // Duplicate the message string to store in the queue
            pthread_mutex_unlock(&receiveQueueMutex);
        }
        else if (messageLen < 0)
        {
            // Handle recvfrom error
        }
        usleep(100000); // Sleep for 100ms
    }
    return NULL;
}
