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

    //create the server socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    //define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.S_un.S_addr = inet_addr("192.168.1.203");

    //bind the socket to our specified IP and port
    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    printf("Server ip address: %d", getsockname(server_socket,(struct sockaddr *) &server_address, (int *) sizeof(server_address)));
    
    while(1)
    {
        listen(server_socket, 5);

        int client_socket;
        struct sockaddr_in client_address;
        client_socket = accept(server_socket, 0, 0);

        char server_message[256] = "Hello, I am the server!";

        send(client_socket,server_message, sizeof(server_message), 0);
    }

    // fclose(server_socket);
    closesocket(server_socket);


    WSACleanup();
    return 0;
}