#include <stdio.h>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

#include <emscripten/val.h>
#include <emscripten/bind.h>

using namespace std;

void resizeObserverCallback(emscripten::val entries, emscripten::val something) {
    cout << "resizeObserverCallback" << endl;
    int count = entries["length"].as<int>();
    for (int i = 0; i < count; ++i) {
        cout << "item " << i << endl;

        emscripten::val entry = entries[i];
        emscripten::val target = entry["target"];
        emscripten::val contentBoxSize = entry["contentBoxSize"];
        emscripten::val contentRect = entry["contentRect"];
        emscripten::val devicePixelContentBoxSize = entry["devicePixelContentBoxSize"];

        cout << "item " << i << " target? " << (target != emscripten::val::undefined() ? "true" : "false") << endl;

        if (contentBoxSize != emscripten::val::undefined()) {
           int width = contentBoxSize["inlineSize"].as<int>();
           int height = contentBoxSize["blockSize"].as<int>();
           cout << "contentBoxSize " << width << " " << height << endl;
        }
        if (contentRect != emscripten::val::undefined()) {
            if (contentRect["inlineSize"] != emscripten::val::undefined()) {
                int width = contentRect["inlineSize"].as<int>();
                int height = contentRect["blockSize"].as<int>();
                cout << "contentRect " << width << height << endl;
            }
        }
        if (devicePixelContentBoxSize != emscripten::val::undefined()) {
            int width = devicePixelContentBoxSize["inlineSize"].as<int>();
            int height = devicePixelContentBoxSize["blockSize"].as<int>();
            cout << "devicePixelContentBoxSize " << width << height << endl;
        }
    }
}

EMSCRIPTEN_BINDINGS(qtResizeObserverCallback) {
    emscripten::function("qtResizeObserverCallback", resizeObserverCallback);
}

int main() {
    cout << "Hello" << endl;

    emscripten::val ResizeObserver = emscripten::val::global("ResizeObserver");
    if (ResizeObserver == emscripten::val::undefined()) {
        cout << "ResizeObserver is undefined" << endl;
        return 0;
    }
    emscripten::val resizeObserver = ResizeObserver.new_(emscripten::val::module_property("qtResizeObserverCallback"));
    if (resizeObserver == emscripten::val::undefined()) {
        cout << "resizeObserver is undefined" << endl;
        return 0;
    }
    resizeObserver.call<void>("observe", emscripten::val::global("resizeTestCanvas"));
    return 0;
}
