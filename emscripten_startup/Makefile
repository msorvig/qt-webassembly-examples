
.PHONY : all

all: makedir default minimal_shell minimal_js qtloader_shell

makedir:
	mkdir -p OUT

default:
	# default build
	emcc main.cpp -o OUT/default.html -std=c++11 -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=\"'Minimal'\"

minimal_shell:
	# minimal shell build
	emcc main.cpp -o OUT/minimal.html --shell-file minimal.html -std=c++11 -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=\"'Minimal'\"

minimal_js:
	# minimal direct js build
	emcc main.cpp -o OUT/minimal_direct.js -std=c++11 -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=\"'Minimal'\"

qtloader_shell:
	# minimal qtloader.js user build
	sed 's/{{{ MODULE_NAME }}}/Minimal/g' qtloader_user.html > OUT/qtloader_user_tmp.html
	emcc main.cpp -o OUT/qtloader_user.html --shell-file OUT/qtloader_user_tmp.html -std=c++11 -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=\"'Minimal'\"
	rm OUT/qtloader_user_tmp.html


