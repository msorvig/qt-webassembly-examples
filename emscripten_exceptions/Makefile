all:
	
	# plain build
	#em++ -g -c main.cpp
	#em++ main.o -g -o main.html
		
	# enable Emscripten emulated exceptions (slow)
	#em++ -g -c main.cpp -sNO_DISABLE_EXCEPTION_CATCHING
	#em++ main.o -g -o main.html -sNO_DISABLE_EXCEPTION_CATCHING

	# enable Emscripten emulated exceptions for spesific functuons. All
	# functions which the exception passes through must be listed.
	#em++ -g -c main.cpp -sEXCEPTION_CATCHING_ALLOWED=_Z6throwsi,_Z7catchesv,_Z12intermediatei
	#em++ main.o -g -o main.html -sEXCEPTION_CATCHING_ALLOWED=_Z6throwsi,_Z7catchesv,_Z12intermediatei

	# enable Wasm exceptions (fast)
	em++ -g -c main.cpp -fwasm-exceptions
	em++ main.o -g -o main.html -fwasm-exceptions
