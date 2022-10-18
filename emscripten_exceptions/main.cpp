#include <stdio.h>

class RAII
{
public:
    RAII() {
        puts("+RAII");
    }
    ~RAII() {
        puts("-RAII");
    }
};

void throws(int i) {
    //RAII rairai;
    printf("throw %d\n", i);
    throw i;
}

void throws(double decoy) {
    
}

void intermediate(int i) {
    RAII rairai;
    throws(i);
}

void catches() {
    try {
        puts("try");
        intermediate(4);
    } catch(...) {
        puts("catch");
    }
    
    puts("throw 5");
    throw 5;
}

int main(int argc, char **argv)
{
    catches();
}