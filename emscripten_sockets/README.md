Minimal UNIX socket echo server/client example for emscripten (WIP)
===================================================================

Mode of operation is that the client makes a TCP connection to localhost:PORT. Emscripten
will then make a WebSockets connection to localhost:PORT and forward/tunnel the tcp messages
on that WebSockets connection. A Websockify WebSockets server is running on localhost:PORT
and makes a TCP connection to the "real" server.

Emscripten code:

    tests/sockets/test_sockets_echo_client.c 
    tests/sockets/test_sockets_echo_server.c 

Blog post on emscripten sockets:

    https://blog.squareys.de/emscripten-sockets/

Implementation Overview
-----------------------

client.c : The echo client
server.c : The echo server

Run "make" to make native and WebAssembly builds:

    - OUT/nativeserver
    - OUT/nativeclient
    - OUT/client.html and client.js

Server: listens on localhost:8888
Websockify: forwards from localhost:8887 -> localhost:8888

    websockify localhost:8887 localhost:8888

Client: connects to
     native     : localhost:8888
     webassembly: localhost:8887

Client
------
The native client uses blocking sockets and sends/receives messages in a loop.

On WebAssembly, the client runs either on the main thread or on a secondary thread.

Main thread: The client runs in O_NONBLOCK non-blocking mode. We can send() messages;
emscripten_set_socket_message_callback() gives us a ready-read notifications, after
which we can recv() them.

Seconary thread: blocking mode can theoretically work, but appears to be non-blocking. send()
returns EAGAIN right after the connection has been made and can be retried. recv() returns
EAGAIN immediately if there is no data. emscripten_set_socket_message_callback() works






