# Note: set QT_BUILD_PATH when running "make"
QT_INLUCDE_PATH=${QT_BUILD_PATH}/include
QT_LIB_PATH=${QT_BUILD_PATH}/lib

main:
	mkdir -p OUT
	
	# build app
	emcc main.cpp -o OUT/qtcore_dynamic.html -g --bind -std=c++17 -s MAIN_MODULE=1 -s WASM_BIGINT=1 -I${QT_INLUCDE_PATH} -L${QT_LIB_PATH} -lQt6Core
	
	# deploy Qt
	cp ${QT_LIB_PATH}/libQt6Core.so  OUT/
