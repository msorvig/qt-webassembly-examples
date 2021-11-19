
.PHONY : all

all: main 

main: main.cpp index.html
	emcc main.cpp -o main.js -std=c++11 -g2 --bind -s ALLOW_MEMORY_GROWTH=1 
