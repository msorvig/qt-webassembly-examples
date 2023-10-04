#include <emscripten.h>
#include <emscripten/bind.h>

void to_infinity()
{
    to_infinity(); // and beyond!
}

int main(int argc, char **argv)
{
    puts("main");
    
    to_infinity();
    
    puts("main done");
}
