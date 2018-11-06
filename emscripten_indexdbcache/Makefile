all: main

main:
	mkdir -p OUT/
	rm -f OUT/index.html && ln -s ../index.html OUT/index.html
	emcc main.cpp -o OUT/main.js -Os -g4 -std=c++11
	shasum -p -a 256 OUT/main.wasm

