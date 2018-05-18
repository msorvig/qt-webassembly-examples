WASM=-s WASM=1
OUT_SUFFIX=wasm

# EXPORT_ALL=1 

# EMCC_FORCE_STDLIBS: We can make the main module carry all stdlibs, however:
# EMCC_FORCE_STDLIBS=1 is broken:
# STDLIBS=EMCC_FORCE_STDLIBS=1
# https://github.com/kripken/emscripten/issues/5436
STDLIBS=

all: main lib_a lib_b

main:
	# main lib with MAIN_MODULE=1. Note that this makes emcc not generate wasm loading code
	# (see emscripten_dynamic.html)
	cp emscripten_dynamic.html OUT/
	${STDLIBS} emcc main.cpp -o OUT/dynamicmain.js -g3 -std=c++11 -s MAIN_MODULE=1 -s WASM=1 -s MODULARIZE=1 -s FORCE_FILESYSTEM=1
lib_a:
	emcc lib_a.cpp -o OUT/lib_a.o -g3 -std=c++11
	emcc OUT/lib_a.o -o OUT/lib_a.${OUT_SUFFIX} -g3 -std=c++11 -s SIDE_MODULE=1 ${WASM}

	# ### emscripten does not generate wasm for versioned output files (like ".wasm.1")
	emcc OUT/lib_a.o -o OUT/lib_a.wasm.1 -g3 -std=c++11 -s SIDE_MODULE=1 -s WASM=1

lib_b:
	emcc lib_b.cpp -o OUT/lib_b.${OUT_SUFFIX} -g3 -std=c++11 -s SIDE_MODULE=1 ${WASM}
	
