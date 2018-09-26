// Code from https://www.binarytides.com/server-client-example-c-sockets-linux/

/*
    C ECHO client example using sockets
*/
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<unistd.h> // close
#include<netdb.h> // gethostbyname

int testConnect()
{
    int sock;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000];
    
#ifdef __EMSCRIPTEN__
    puts("Note: Work in progress. Shold get as far as making a http request to example.com");
    const char *hostname = "example.com";
#else
    const char *hostname = "localhost";
#endif    
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
    
    struct hostent *h = gethostbyname(hostname);
    if (h == 0) 
    {
        puts("host lookup failed");
        return 0;
    }
    
    struct in_addr  **pptr;
    pptr = (struct in_addr **)h->h_addr_list;
    while( *pptr != NULL ) {
        printf("ip address: %s\n", inet_ntoa(**(pptr++)));
    }
     
    server.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)h->h_addr));
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected\n");
     
    //keep communicating with server
    while(1)
    {
        printf("Enter message : ");
        scanf("%s" , message);
         
        //Send some data
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
         
        //Receive a reply from the server
        if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
            break;
        }
         
        puts("Server reply :");
        puts(server_reply);
    }
     
    close(sock);
    return 0;
}
 
int main(int argc , char *argv[])
{
   return testConnect();
}