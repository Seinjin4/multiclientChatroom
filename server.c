#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <sys/types.h>

#ifdef __WIN32__
    #include <winsock2.h>
#else
    #include <sys/socket.h> // Linux specific i think
    #include <netinet/in.h>
#endif

struct client
{
    int socket;
    struct sockaddr_in;
};

struct client clients[5];
int client_count = 0;

pthread_t recv_send_thread_id[5];
pthread_t newcomer_thread_id;

int server_socket;

void* recv_send(void * arg)
{
    struct client curr_client = *(struct client*) arg;

    printf("Client %d receive/Send thread is running. Listening for messages...\n", &curr_client);

    while(1)
    {

        char message[256];
        if(recv(curr_client.socket, message, sizeof(message), 0) == -1)
        {
            printf("Client disconnected - %d\n", &curr_client);
            shutdown(curr_client.socket, 0);
            pthread_exit(0);
        }

        printf("Client %d: %s\n", &curr_client, message);

        for(int i=0; i < client_count; i++)
        {
            char conn_message[256];
            if(&curr_client != &clients[i])
            {
                sprintf(conn_message, "Client %d: %s", &curr_client, message);
                send(clients[i].socket, conn_message, sizeof(conn_message), 0);
            }
        }
    }
}

void* server_listening(void* arg)
{
    printf("Newcomer thread is running. Listening for clients...\n");

    while(1)
    {
        listen(server_socket, 5);

        struct client new_client;
        new_client.socket = accept(server_socket, 0, 0);

        printf("New client connected - %d\n", &new_client);

        char server_message[256] = "\n Hello, welcone to the chatroom!\n";

        send(new_client.socket, server_message, sizeof(server_message), 0);

        pthread_create(&recv_send_thread_id[client_count], NULL, recv_send, (void *) &clients[client_count]);

        clients[client_count] = new_client;
        client_count++;
    }
}


int main(int argc,char **argv) // [0] - filename, [1] - ipv4, [2] - port
{

    WORD wVersionRequested = MAKEWORD(2,2); // Without this error 10092 occurs
    WSADATA wsaData;

    int err = WSAStartup(wVersionRequested, &wsaData); 
    if(err != 0)
    {
        printf("\n ERROR: An issue occured while starting WSA. Error code: %d \n\n", err);  
        WSACleanup();
        return 0;      
    }

    //create the server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    //define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    int server_port;
    sscanf(argv[2],"%d",&server_port);
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.S_un.S_addr = inet_addr(argv[1]);

    //bind the socket to our specified IP and port
    if(bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == 0)
    {
        printf("Server %s:%d is running...\n\n", argv[1], server_port);
    }
    else
    {
        printf("\n ERROR: An issue occured while binding. Error code: %d \n\n", WSAGetLastError());     
        WSACleanup();
        return 0;   
    }

    int th_err = pthread_create(&newcomer_thread_id, NULL, server_listening, NULL); 
    if(th_err != 0)
    {
        printf("\n ERROR: Issue while creating newcomer_thread: %d \n\n", th_err);     
    }

    pthread_join(newcomer_thread_id, NULL);

    // fclose(server_socket);
    closesocket(server_socket);

    WSACleanup();
    return 0;
}