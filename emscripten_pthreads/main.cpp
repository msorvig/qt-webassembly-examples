#include <stdio.h>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

#include <emscripten.h>
#include <emscripten/html5.h>

using namespace std;

int main() {
    printf("hello, world from main()!\n");

    // fall off main: either keep running or exit with code.
    //::emscripten_exit_with_live_runtime();
    emscripten_force_exit(0);
    return 0;
}
