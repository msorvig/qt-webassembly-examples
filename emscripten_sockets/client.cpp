// Code from https://www.binarytides.com/server-client-example-c-sockets-linux/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <thread>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>
using namespace emscripten;
#endif

int createSocketAndConnect(bool nonblocking)
{
    int sock;
    struct sockaddr_in server;
    const char *hostname = "localhost";
#ifdef __EMSCRIPTEN__
    uint16_t port = 8887; // websockify maps from 8887 -> 8888
#else
    uint16_t port = 8888;
#endif
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
        return -1;
    }
    puts("Socket created");
    
    if (nonblocking)
        fcntl(sock, F_SETFL, O_NONBLOCK);

    struct hostent *h = gethostbyname(hostname);
    if (h == 0)  {
        puts("host lookup failed");
        return -1;
    }
    
    struct in_addr  **pptr;
    pptr = (struct in_addr **)h->h_addr_list;
    while( *pptr != NULL ) {
        printf("ip address: %s\n", inet_ntoa(**(pptr++)));
    }
     
    server.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)h->h_addr));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    
    // Connect to remote server, on Emscripten this won't block and
    // we get EINPROGRESS.
    int res = connect(sock , (struct sockaddr *)&server , sizeof(server));
    if (res == -1 && errno != EINPROGRESS) {
        perror("Connect failed. Error");
        return 1;
    }

    return sock;
}

int testConnect()
{
    int sock = createSocketAndConnect(true);
    if (sock == -1)
        return -1;
     
     
    puts("Connected\n");
     
    //keep communicating with server
    while(1)
    {
        const char *message = "are you ready to hear a TCP joke\0";
        int res = send(sock, message , strlen(message) + 1 , 0);
        if (res != 0) {
            printf("res %d errno %d \n", res, errno);
            break;
        }
         
        //Receive a reply from the server
        char server_reply[2000];
        if( recv(sock, server_reply, 2000 , 0) < 0)
        {
            perror("recv failed");
            break;
        }
    }
     
    close(sock);
    return 0;
}

void sendMessage(int socket)
{
    // Send some data
    const char *message = "are you ready to hear a TCP joke\0";
    if (send(socket, message, strlen(message) + 1, 0) < 0) {
        puts("Send failed");
    }
}

void receiveMessage(int socket)
{
    char server_reply[2000];
    if (recv(socket, server_reply , 2000 , 0) < 0) {
        puts("recv failed");
        return;
    }
     
    puts("Server reply :");
    puts(server_reply);
}

#ifdef __EMSCRIPTEN__
int g_socket = 0;

void readyRead(int fd, void* ) {
  printf("%d readyRead\n", fd);
  if (g_socket)
      receiveMessage(fd);
}

void run_connect()
{
    puts("run_connect");
    if (g_socket)
        return;
    
    emscripten_set_socket_message_callback(nullptr, readyRead);
    g_socket = createSocketAndConnect(true);
}

void run_sendMessage()
{
    puts("run_sendMessage");
    if (g_socket <= 0) {
        puts("No socket");
        return;
    }

    sendMessage(g_socket);
}

void run_sendReceiveInThread()
{
    emscripten_set_socket_message_callback(nullptr, readyRead);

    std::thread *t = new std::thread([](){
        puts("run_sendReceiveInThread");
        
        int sock = createSocketAndConnect(false);
        if (sock == -1)
            return;
        
        puts("send");
        
        while (1) {
            const char *message = "are you ready to hear a TCP joke\0";
            int res = send(sock, message , strlen(message) + 1 , 0);
            if (res < 0) {
                printf("send %d errno %d \n", res, errno);
                perror("err");
            } else {
                break;
            }
        }
        
        puts("recv");

        while (1) {
            char server_reply[2000];
            int res = recv(sock, server_reply, 2000 , 0);
            if (res < 0) {
                printf("recv %d errno %d \n", res, errno);
                perror("err");
            } else {
                break;
            }
        }
        
        puts("send again");
        
        while (1) {
            const char *message = "are you ready to hear a TCP joke\0";
            int res = send(sock, message , strlen(message) + 1 , 0);
            if (res < 0) {
                printf("send %d errno %d \n", res, errno);
            } else {
                break;
            }
        }

    });
}


EMSCRIPTEN_BINDINGS(my_module) {
    function("run_connect", &run_connect);
    function("run_sendMessage", &run_sendMessage);
    function("run_sendReceiveInThread", &run_sendReceiveInThread);
};


#endif
 
int main(int argc , char *argv[])
{
#ifdef __EMSCRIPTEN__

#else
   return testConnect();
#endif   
}