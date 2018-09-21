all: main

main:
	mkdir -p OUT/
	rm -f OUT/localfiles.html && ln -s ../localfiles.html OUT/localfiles.html
	emcc main.cpp -o OUT/main.js -O3 -std=c++11 --bind -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']"
