all: main

main:
	emcc main.cpp -o main.js --bind -Os -g4 -std=c++11 -s ASYNCIFY=1 -s 'ASYNCIFY_IMPORTS=["qt_suspend"]'
