#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>

#ifdef __WIN32__
    #include <winsock2.h>
#else
    #include <sys/socket.h> // Linux specific i think
    #include <netinet/in.h>
#endif

int main(int argc, char** argv) // [0] - filename, [1] - ipv4, [2] - port
{
    WORD wVersionRequested;
    WSADATA wsaData;

    int err = WSAStartup(wVersionRequested, &wsaData);
    if(err != 0)
    {
        printf("ERROR: An issue occured with the WSA startup. Error code: %d \n\n", err);        
    }

    int network_socket;
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
    }

    // receive data from the server
    char server_response[256];
    recv(network_socket, server_response, sizeof(server_response), 0);

    // print out the server's response
    printf("Server: %s \n", server_response);


    scanf("%s");
    // closing the socket
    closesocket(network_socket);
    // close(network_socket);

    WSACleanup();
    return 0;
}