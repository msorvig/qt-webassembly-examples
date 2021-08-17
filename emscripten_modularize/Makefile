all:
	mkdir -p OUT
	cp -f emscripten_modularize.html OUT
	em++ main.cpp -o OUT/main.js -s --bind MODULARIZE=1 -s EXPORT_NAME=main -s EXPORTED_RUNTIME_METHODS='[ENV]'