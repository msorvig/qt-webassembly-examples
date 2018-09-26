all:
	mkdir -p OUT
	clang client.c -o OUT/nativeclient
	clang server.c -o OUT/nativeserver
	emcc client.c -o OUT/client.html -s WASM=1