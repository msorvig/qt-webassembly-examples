all: main

main:
	mkdir -p OUT/
	rm -f OUT/asyncify.html && ln -s ../asyncify.html OUT/asyncify.html
	emcc main.cpp -o OUT/main.js --bind -Os -g4 -std=c++11  -s WASM=1 -s ASYNCIFY=1
