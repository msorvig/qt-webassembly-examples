all: main

main:
	emcc main.cpp -o main_asyncify_1.js --bind -g3 -std=c++11 -s ASYNCIFY=1 -sMODULARIZE -sEXPORT_NAME=Module_asyncify_1 -sASYNCIFY_DEBUG=0
	emcc main.cpp -o main_asyncify_2.js --bind -g3 -std=c++11 -s ASYNCIFY=2 -sMODULARIZE -sEXPORT_NAME=Module_asyncify_2 -sASYNCIFY_DEBUG=0
