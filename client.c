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

int network_socket;

pthread_t send_thread_id;
pthread_t recv_thread_id;

char messages[256][256];
int message_count = 0;

void add_message_to_cache(char message[])
{
    strcpy(messages[message_count % 256], message);
    message_count++;
}

void print_all_messages()
{
    system("cls");
    for(int i=0; i<message_count;i++)
    {
        printf("%s \n", messages[i]);
    }
}

void* send_thread(void* arg)
{
    char message[256];
    while(1)
    {
        char message[256];
        scanf("%s", message);
        
        if(send(network_socket, message, sizeof(message), 0) > sizeof(message)) // ok if send returns size of message or less
        {
            printf("ERROR: There was an issue sending the message. Error code: %d\n", WSAGetLastError());
        }
    }
}

void* recv_thread(void* arg)
{
    while(1)
    {
        char message[256];
        if(recv(network_socket, message, sizeof(message), 0) == -1 )
        {
            printf("You got disconnected from the server. Enter any key to continue...");
            scanf("%s", 0);
            pthread_exit(0);
        } 

        add_message_to_cache(message);
        print_all_messages();
        // printf("%s\n", message);
    }
}

int main(int argc, char** argv) // [0] - filename, [1] - ipv4, [2] - port
{
    WORD wVersionRequested;
    WSADATA wsaData;

    int err = WSAStartup(wVersionRequested, &wsaData);
    if(err != 0)
    {
        printf("ERROR: An issue occured with the WSA startup. Error code: %d \n\n", err);        
    }

    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // specify address for the socket

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    int server_port;
    sscanf(argv[2],"%d",&server_port);
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.S_un.S_addr = inet_addr(argv[1]);
    
    if(connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == 0)
    {
        printf("Successfully connected to the server...\n");
    }
    else
    {
        printf("ERROR: An issue occured while making the connection. Error code: %d \n\n", WSAGetLastError());

        WSACleanup();
        return 0;
    }

    // receive data from the server
    char server_response[256];
    recv(network_socket, server_response, sizeof(server_response), 0);

    // print out the server's response
    printf("Server: %s \n", server_response);

    int th_err = pthread_create(&send_thread_id, 0, send_thread, 0);
    if(th_err != 0)
    {
        printf("\n ERROR: Issue while creating send thread: %d \n\n", th_err);     
    }

    th_err = pthread_create(&recv_thread_id, 0, recv_thread, 0);
    if(th_err != 0)
    {
        printf("\n ERROR: Issue while creating receive thread: %d \n\n", th_err);     
    }

    pthread_join(recv_thread_id, NULL);
    pthread_join(send_thread_id, NULL);


    // closing the socket
    closesocket(network_socket);
    // close(network_socket);

    WSACleanup();
    return 0;
}