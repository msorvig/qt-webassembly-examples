
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
	emcc main.cpp -o OUT/default.js -std=c++11 -s USE_PTHREADS=2 -s WASM=1 -s MODULARIZE=1 -s ALLOW_MEMORY_GROWTH=1 -s WASM_MEM_MAX=1610612736
	
	# note 5: default.js uses Atomics unconditionally, making the of USE_PTHREADS=2 feature somewhat less useful,
	#		  since at least FireFox disables Atomics along with SharedArrayBuffer
	# note 6: loading the module errors out with "TypeError: invalid array type for the operation" on a Atomics.store
	#         this looks like https://github.com/WebAssembly/binaryen/issues/1419
