all: main

main:
	mkdir -p OUT/
	rm -f OUT/wasmeval.html && ln -s ../wasmeval.html OUT/wasmeval.html

	# Generate minimal wasm and wast using emscripten options -s SIDE_MODULE=1 and ONLY_MY_CODE=1:
	emcc main.cpp -o OUT/main.wasm -Os -g4 -std=c++11 -s SIDE_MODULE=1 -s ONLY_MY_CODE=1 -s WASM=1
