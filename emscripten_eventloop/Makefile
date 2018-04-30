all: main

main:
	mkdir -p OUT/
	rm -f OUT/eventloop.html && ln -s ../eventloop.html OUT/eventloop.html
	emcc main.cpp -o OUT/main.js -Os -g4 -std=c++11  -s WASM=1 -s EXPORTED_FUNCTIONS='["_update_loop_timing1", "_update_loop_timing2", "_main"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]'
