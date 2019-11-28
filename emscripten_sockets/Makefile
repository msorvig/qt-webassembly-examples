all:
	mkdir -p OUT
	clang++ client.cpp -o OUT/nativeclient
	clang++ server.cpp -o OUT/nativeserver
	em++ client.cpp -o OUT/client.js  --bind -std=c++11 -s WASM=1 -g2 -s SOCKET_DEBUG=1 -s USE_PTHREADS=1
	ln -sf ../client.html OUT/client.html
	