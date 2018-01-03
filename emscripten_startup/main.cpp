#include <stdio.h>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <emscripten.h>
#include <emscripten/html5.h>

using namespace std;

EMSCRIPTEN_WEBGL_CONTEXT_HANDLE createContext(const char *canvasId)
{
    EmscriptenWebGLContextAttributes attributes;

    emscripten_webgl_init_context_attributes(&attributes); // Pupulate with default attributes
    attributes.preferLowPowerToHighPerformance = false;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context(canvasId, &attributes);
    cout << "create context " << context << endl;

    return context;
}

EMSCRIPTEN_WEBGL_CONTEXT_HANDLE g_context = 0;

void render()
{
    const char *canvasId = 0;

    if (!g_context)
        g_context = createContext(canvasId);

    emscripten_webgl_make_context_current(g_context);

    glClearColor(0.1f, 0.5f, 0.5, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Recurse to trigger stack overflow crash
    //render();
}

void noop()
{

}

int main() {
    printf("hello, world from main()!\n");

    // Comment in to abort
    // EM_ASM(Module.abort("Unexpected this is"););

    render();

    emscripten_set_main_loop(noop, 0, 0);

    // fall off main: either keep running or exit with code.
    //::emscripten_exit_with_live_runtime();
    emscripten_force_exit(0);
    return 0;
}
