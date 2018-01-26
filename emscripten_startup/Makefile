
.PHONY : all

all: makedir default minimal_shell minimal_js qtloader_user_managed qtloader_user_external

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

qtloader_user_managed:
	# minimal qtloader.js user build
	sed 's/{{{ MODULE_NAME }}}/Minimal/g' qtloader_user_managed.html > OUT/qtloader_user_managed_tmp.html
	emcc main.cpp -o OUT/qtloader_user_managed.html --shell-file OUT/qtloader_user_managed_tmp.html -std=c++11 -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=\"'Minimal'\"
	rm -f OUT/qtloader_user_managed_tmp.html

qtloader_user_external:
	# minimal qtloader.js user build
	sed 's/{{{ MODULE_NAME }}}/Minimal/g' qtloader_user_external.html > OUT/qtloader_user_external_tmp.html
	emcc main.cpp -o OUT/qtloader_user_external.html --shell-file OUT/qtloader_user_external_tmp.html -std=c++11 -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=\"'Minimal'\"
	rm -f OUT/qtloader_user_external_tmp.html


