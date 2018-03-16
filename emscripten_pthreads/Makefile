
.PHONY : all

all: makedir main 

makedir:
	mkdir -p OUT
	cp index.html OUT

main:
	# note 1: Generating an html file does not work: "AssertionError: no --separate-asm means no client code mods are possible"
	# note 2: setting ALLOW_MEMORY_GROWTH triggers "ERROR:root:If pthreads and memory growth are enabled, WASM_MEM_MAX must be set"
	# note 3: setting WASM_MEM_MAX to 2GB asserts: "[wasm-validator error in module] unexpected false: shared memory must have max size"
	# note 4: WASM_MEM_MAX=1.5GIB appears to work.
	emcc main.cpp -o OUT/default.js -std=c++11  -s PTHREAD_POOL_SIZE=4 -s USE_PTHREADS=2 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s WASM_MEM_MAX=1610612736
	
	# note 5: default.js uses Atomics unconditionally, making the of USE_PTHREADS=2 feature somewhat less useful,
	#		  since at least FireFox disables Atomics along with SharedArrayBuffer
	# note 6: loading the module errors out with "TypeError: invalid array type for the operation" on a Atomics.store
	#         this looks like https://github.com/WebAssembly/binaryen/issues/1419
	# note 7: Previous error seems to be a browser error, Firefox Nightly (61) works, but now
	#		  "pthread-main.js onmessage() captured an uncaught exception: TypeError: Argument 1 is not valid for any
	#		  of the 1-argument overloads of URL.createObjectURL."
	# note 8: According to https://github.com/kripken/emscripten/pull/5016, USE_PTHREADS does not work in MODULARIZE=1 mode
