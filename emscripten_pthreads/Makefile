
.PHONY : all

all: makedir main 

makedir:
	mkdir -p OUT
	cp index.html OUT

main:
# note 1: Generating an html file does not work: "AssertionError: no --separate-asm means no client code mods are possible"
# note 2: According to https://github.com/kripken/emscripten/pull/5016, USE_PTHREADS does not work in MODULARIZE=1 mode
# note 3: ALLOW_MEMORY_GROWTH is now not supported: ERROR:root:Memory growth is not yet supported with pthreads
# note 4: Browsers appear to limit the maximum initial memory size to 1GB, set TOTAL_MEMORY accordingly
# note 5: The program hangs if the number of active threads exceed PTHREAD_POOL_SIZE.
	emcc main.cpp -o OUT/default.js -std=c++11 -s PTHREAD_POOL_SIZE=2 -s USE_PTHREADS=1 -s TOTAL_MEMORY=1GB
