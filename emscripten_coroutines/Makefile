cross_platform_main: cross_platform_main.cpp
	clang++ cross_platform_main.cpp -o cross_platform_main -std=c++2a  -stdlib=libc++

main: main.cpp
	em++ main.cpp -o main.html -std=c++2a --bind 
	em++ main.cpp -o main_opt.html -std=c++2a --bind -O2