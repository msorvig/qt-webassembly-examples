all: 
	# build archive
	emcc interop_archive.cpp -o interop_archive.o -std=c++11
	rm -f interop_archive.a
	emar cqs interop_archive.a interop_archive.o

	# build main
	emcc interop.cpp  interop_archive.a -o interop.html -std=c++11

# alternative with EXPORTED_FUNCTIONS
#	emcc interop.cpp  interop_archive.a -o interop.html -s EXPORTED_FUNCTIONS='["_main", "_sayHelloFromArchive"]' -std=c++11
	
