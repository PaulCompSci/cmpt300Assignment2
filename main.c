#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>     // For string functions
#include <unistd.h>     // For usleep
#include <sys/socket.h> // For sendto and recvfrom
#include <netinet/in.h> // For struct sockaddr_in
#include "list.h"
#include <arpa/inet.h>
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
extern void *keyboard_input_thread(void *arg);
extern void *screen_output_thread(void *arg);
extern void *udp_send_thread(void *arg);
extern void *udp_receive_thread(void *arg);

// Global list accessible by all threads
List *messageList;

// Define the default port number
#define DEFAULT_PORT 5000

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s [local port] [remote IP] [remote port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int localPort = atoi(argv[1]);
    char *remoteIP = argv[2];
    int remotePort = atoi(argv[3]);

    int port = DEFAULT_PORT; // Default port number
    
    // Check if a command-line argument for the port number is provided
    if (argc > 1) {
        port = atoi(argv[1]); // Convert the command-line argument to an integer
    }

    memset(&remoteAddr, 0, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(port); // Set the port number
    if (inet_pton(AF_INET, "127.0.0.1", &remoteAddr.sin_addr) <= 0)
    {
        // Handle error
        return -1;
    }

    memset(&remoteAddr, 0, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(remotePort);
    if (inet_pton(AF_INET, remoteIP, &remoteAddr.sin_addr) <= 0) {
        perror("inet_pton() failed");
        return -1;
    }


    pthread_mutex_init(&sendQueueMutex, NULL);
    pthread_mutex_init(&receiveQueueMutex, NULL);
    pthread_mutex_init(&listMutex, NULL);

    printf("this is a good start!\n"); // Add a newline character to the end of the message

    // Create a UDP socket for receiving messages
    udpRecvSockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpRecvSockfd < 0) {
        perror("Error creating socket");
        return -1;
    }

    // Set up the local address structure
    struct sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on any network interface
    localAddr.sin_port = htons(port); // Listen on the defined port

    int optval = 1;
    if (setsockopt(udpRecvSockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        close(udpRecvSockfd);
        return -1;
    }   
    // Bind the socket
    if (bind(udpRecvSockfd, (struct sockaddr *)&localAddr, sizeof(localAddr)) < 0) {
        perror("Bind failed");
        close(udpRecvSockfd);
        return -1;
    }

    // Initialize the list
    messageList = List_create();

    // Check for list creation failure
    if (!messageList)
    {
        // Handle error
        return -1;
    }

    // Create threads
    pthread_t tid_keyboard, tid_screen, tid_udp_send, tid_udp_receive;
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

// void *keyboard_input_thread(void *arg)
// {
//     char message[256];
//     while (fgets(message, sizeof(message), stdin) && running)
//     {
//         char *pos;
//         if ((pos = strchr(message, '\n')) != NULL)
//             *pos = '\0'; // Remove newline character

//         pthread_mutex_lock(&sendQueueMutex);
//         if (strcmp(message, "exit") == 0)
//             running = 0;                         // Exit command
//         List_append(sendQueue, strdup(message)); // Duplicate message to heap
//         pthread_mutex_unlock(&sendQueueMutex);
//     }
//     return NULL;
// }
// void *screen_output_thread(void *arg)
// {
//     while (running)
//     {
//         pthread_mutex_lock(&listMutex);
//         char *message = (char *)List_trim(messageList); // Assume List_trim returns last item
//         pthread_mutex_unlock(&listMutex);

//         if (message)
//         {
//             printf("%s\n", message);
//             free(message); // Assume message was dynamically allocated
//         }
//         usleep(100000); // Sleep for 100ms to avoid busy waiting
//     }
//     return NULL;
// }
// void *udp_send_thread(void *arg)
// {
//     while (running)
//     {
//         pthread_mutex_lock(&sendQueueMutex);
//         char *message = (char *)List_trim(sendQueue); // Get message to send
//         pthread_mutex_unlock(&sendQueueMutex);

//         if (message)
//         {
//             ssize_t bytes_sent = sendto(udpSendSockfd, message, strlen(message), 0,
//                                         (struct sockaddr *)&remoteAddr, sizeof(remoteAddr));
//             if (bytes_sent < 0)
//             {
//                 // Handle error
//             }
//             free(message); // Cleanup
//         }
//         usleep(100000); // Sleep for 100ms
//     }
//     return NULL;
// }
// void *udp_receive_thread(void *arg)
// {
//     char buffer[1024];
//     while (running)
//     {
//         ssize_t messageLen = recvfrom(udpRecvSockfd, buffer, sizeof(buffer) - 1, 0, NULL, NULL);
//         if (messageLen > 0)
//         {
//             buffer[messageLen] = '\0'; // Null-terminate the received message
//             pthread_mutex_lock(&receiveQueueMutex);
//             List_append(receiveQueue, strdup(buffer)); // Duplicate the message string to store in the queue
//             pthread_mutex_unlock(&receiveQueueMutex);
//         }
//         else if (messageLen < 0)
//         {
//             // Handle recvfrom error
//         }
//         usleep(100000); // Sleep for 100ms
//     }
//     return NULL;
// }
