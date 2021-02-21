#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>

#ifdef __WIN32__
    #include <winsock2.h>
#else
    #include <sys/socket.h> // Linux specific i think
    #include <netinet/in.h>
#endif

int main()
{
    WORD wVersionRequested;
    WSADATA wsaData;

    int err = WSAStartup(wVersionRequested, &wsaData);
    if(err != 0)
    {
        printf("ERROR: An issue occured while making the connection. Error code: %d \n\n", err);        
    }

    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // specify address for the socket

    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.S_un.S_addr = inet_addr("192.168.1.203");

    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    
    // checking if everything is ok woth the connection
    if(connection_status == -1)
        printf("ERROR: An issue occured while making the connection. Error code: %d \n\n", WSAGetLastError());

    // receive data from the server
    char server_response[256];
    recv(network_socket, server_response, sizeof(server_response), 0);

    // print out the server's response
    printf("Server: %s \n", server_response);

    // closing the socket
    closesocket(network_socket);
    // close(network_socket);

    WSACleanup();
    return 0;
}