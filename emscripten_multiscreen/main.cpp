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

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

EMSCRIPTEN_WEBGL_CONTEXT_HANDLE createContext(const char *canvasId)
{

    EmscriptenWebGLContextAttributes attributes;

    emscripten_webgl_init_context_attributes(&attributes); // Pupulate with default attributes

    attributes.preferLowPowerToHighPerformance = false;

#if 0
    attributes.alpha = true;
    attributes.depth = true;
    attributes.stencil = false;
    attributes.antialias = true;
    attributes.premultipliedAlpha = true;
    attributes.preserveDrawingBuffer = false;
    attributes.failIfMajorPerformanceCaveat = false;
    attributes.majorVersion = 1;
    attributes.minorVersion = 0;
    attributes.enableExtensionsByDefault = true;
#endif

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context(canvasId, &attributes);
    cout << "create context " << context << endl;

    EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(context);
    cout << "make current context " << (res == EMSCRIPTEN_RESULT_SUCCESS) << endl;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE current = emscripten_webgl_get_current_context();
    cout << "get current context " << current << endl;

    const char *extensions = (const char*)glGetString(GL_EXTENSIONS);
    std::vector<std::string> exts = split(extensions, ' ');
    for (size_t i = 0; i < exts.size(); ++i)
    {
        EM_BOOL supported = emscripten_webgl_enable_extension(context, exts[i].c_str());
    //    cout << "extention " << exts[i] << " " << supported << endl;
    }

    void *data = nullptr;
    bool capture = true;
    auto callback = [](int eventType, const void *reserved, void *userData) -> EM_BOOL
    {
        switch (eventType) {
            case EMSCRIPTEN_EVENT_WEBGLCONTEXTLOST:
                cout << "context lost";
            break;
            case EMSCRIPTEN_EVENT_WEBGLCONTEXTRESTORED:
                cout << "context restored";
            break;
            default:
                cout << "unexpected this is";
            break;
        };
        return true;
    };

    emscripten_set_webglcontextlost_callback(canvasId, data, capture, callback);
    emscripten_set_webglcontextrestored_callback(canvasId, data, capture, callback);

    return context;
}


void printComputedStyleWidth(const char *targetId)
{
    EM_ASM_({
        var canvasId = Module.Pointer_stringify($0);
        var canvas = document.getElementById(canvasId);
        var style = window.getComputedStyle(canvas);
        console.log("computed width " + style.getPropertyValue("width"));
    }, targetId);
}

void printContextInfo(const char *canvasId, EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context)
{
    // Get canvas logical size
    double css_width;
    double css_height;
    emscripten_get_element_css_size(canvasId, &css_width, &css_height);

    // window.devicePixelRatio returns the scale factor for the display.
    // On Chrome and Firefox the current page zoom is also factored in;
    // this allows rendering more detail when zoomed in. Safari soes not
    // do this.
    double window_dpr = EM_ASM_DOUBLE({
        return window.devicePixelRatio;
    });

    // New window.visualViewport API will give access the viewport zoom factor (?)
    double viewport_scale = EM_ASM_DOUBLE({
        if (typeof window.visualViewport === "undefined")
            return -1;
        return window.visualViewport.scale;
    });

    int draw_width;
    int draw_height;
    emscripten_webgl_get_drawing_buffer_size(context, &draw_width, &draw_height);

    double canvas_dpr = draw_width / css_width;

    cout << " css size " << css_width << " " << css_height
         << " draw size " << draw_width << " " << draw_height
         << " canvas computed dpr " << canvas_dpr
         << " window dpr " << window_dpr
         << " visualViewport scale " << viewport_scale
         << endl;
}

int g_canvasCount = 4;
const char *g_canvasIds[] = { "canvas-a", "canvas-b", "canvas-c", "canvas-d" };
EMSCRIPTEN_WEBGL_CONTEXT_HANDLE g_contexts[] = {0, 0, 0, 0};

void render(int canvasIndex)
{
    if (!g_contexts[canvasIndex])
        g_contexts[canvasIndex] = createContext(g_canvasIds[canvasIndex]);

    emscripten_webgl_make_context_current(g_contexts[canvasIndex]);
    printContextInfo(g_canvasIds[canvasIndex], g_contexts[canvasIndex]);

    glClearColor(0.2f, 0.5f, 0.3f + (canvasIndex / (4 * 3.0)), 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void noop()
{

}

void qt_set_canvas_size(const char *targetId, double width, double height)
{
    EM_ASM_({
        var canvasId = Module.Pointer_stringify($0);
        var canvas = document.getElementById(canvasId);
        canvas.width = $1;
        canvas.height = $2;
    }, targetId, width, height);
}

int main() {
    printf("hello, world!\n");

    void *data = nullptr;
    bool capture = true;
    auto callback = [](int eventType, const EmscriptenUiEvent *uiEvent, void *userData) -> EM_BOOL
    {
        cout << "emscripten resize callback" << endl;

        double window_dpr = EM_ASM_DOUBLE({
            return window.devicePixelRatio;
        });

        // The HTML canvas has two sizes: the CSS size and the canvas render size.
        // The CSS size is determined according to standard CSS rules, while the
        // render size is set using the "width" and "height" attributes. The render
        // size must be set manually and is not auto-updated on CSS size change.
        // Setting the render size to a value larger than the CSS size enables high-dpi
        // rendering.
        //
        // As a special case, the canvas render size will be used as the CSS size
        // if no CSS size is set.
        //
        // For WebGL canvases the final buffer size may be different than the requested
        // render size due to GPU limitations. Use emscripten_webgl_get_drawing_buffer_size
        // to get the actual size.

        // Set the canvas render size, factoring in window.devicePixelRatio.
        for (int i = 0; i < g_canvasCount; ++i) {
            const char *canvasId = g_canvasIds[i];

            double css_width;
            double css_height;
            emscripten_get_element_css_size(canvasId, &css_width, &css_height);

            // FIXME: the css size returned above might in fact be the canvas render
            // size, if no CSS size is set. This means that setting the canvas render
            // size below will feed back into the next read of the CSS size, which is
            // not what we want.

            double pixel_width = css_width * window_dpr;
            double pixel_height = css_height * window_dpr;

            cout << "set size " << pixel_width << " " << pixel_height << endl;
            qt_set_canvas_size(canvasId, pixel_width, pixel_height);
        }

        for (int i = 0; i < g_canvasCount; ++i) {
            render(i);
        }

        printContextInfo(g_canvasIds[0], g_contexts[0]);
        return true;
    };
    emscripten_set_resize_callback(0, data, capture, callback);

    render(0);
    render(1);
    render(2);
    render(3);

    emscripten_set_main_loop(noop, 0, 0);

    return 0;
}
