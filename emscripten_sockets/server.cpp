
// Code from https://www.binarytides.com/server-client-example-c-sockets-linux/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    uint16_t port = 8888;
         
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    const int en = 1;
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(int)) < 0)
        puts("setsockopt(SO_REUSEADDR) failed");

    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    printf("bind done on port %d...\n", port);
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    printf("Waiting for incoming connections on port %d...\n", port);
    c = sizeof(struct sockaddr_in);
     
    //accept connection from an incoming client
    while (1) {
        
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0)
        {
            perror("accept failed");
            return 1;
        }
        puts("Connection accepted");
     
        //Receive a message from client
        char client_message[2000];
        while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
        {
            //Send the message back to client
            printf("Got client message %s\n", client_message);
             write(client_sock , client_message , strlen(client_message));
        }
     
        if(read_size == 0)
        {
            puts("Client disconnected");
            fflush(stdout);
        }
        else if(read_size == -1)
        {
            perror("recv failed");
        }
    }
     
    return 0;
}