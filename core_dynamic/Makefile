# note: this hardcodes a spesific Qt build location!
QTPATH=../../qt-webassembly-build-wasm-dynamic/qtbase
INLUCDES=-I${QTPATH}/include
# Qt required stdlibs: libc,libcxx,libcxxabi,compiler-rt,html5
STDLIBS=libc,libcxx

main:
	mkdir -p OUT
	cp qtcore_dynamic.html OUT/

	ln -sf ../${QTPATH}/lib/libQt5Core.wasm OUT/libQt5Core.wasm
	EMCC_FORCE_STDLIBS="${STDLIBS}" emcc main.cpp -o OUT/qtcore_dynamic.js -g4 -std=c++11 -s WASM=1 -s MODULARIZE=1 -s USE_ZLIB=1 -s MAIN_MODULE=1 -s DEMANGLE_SUPPORT=1  -s FORCE_FILESYSTEM=1 ${INLUCDES}
