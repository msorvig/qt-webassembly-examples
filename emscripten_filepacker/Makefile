
.PHONY : all

all:
	mkdir -p OUT
	cp filepacker.html OUT/
	emcc main.cpp -o OUT/main.js -std=c++11 -g -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME="filepacker" --preload-file assets
