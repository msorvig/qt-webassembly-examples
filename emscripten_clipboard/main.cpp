#include <stdio.h>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

using namespace emscripten;

void noop()
{

}

int keyboard_cb(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
{
    std::cout << "keyboard " << keyEvent->metaKey << " " <<  keyEvent->key << " " << std::endl;

    bool mac = false;
    if (mac ? keyEvent->metaKey : keyEvent->ctrlKey && strcmp(keyEvent->key, "x") == 0)
        return 0;
    if (mac ? keyEvent->metaKey : keyEvent->ctrlKey && strcmp(keyEvent->key, "c") == 0)
        return 0;
    if (mac ? keyEvent->metaKey : keyEvent->ctrlKey && strcmp(keyEvent->key, "v") == 0)
        return 0;

    return 0;
}

void installEventhandlers()
{
    val navigator = val::global("navigator");
    val window = val::global("window");
    val clipboard = navigator["clipboard"];
    bool hasClipboardSupport = (!clipboard.isUndefined() && !clipboard["readText"].isUndefined());
    
    std::cout << "has clipboard " << !clipboard.isUndefined();

    val canvasA = val::global("canvas-a");
    val canvasB = val::global("canvas-b");
    canvasA.call<void>("addEventListener", std::string("paste"), val::module_property("clipBoardPaste"), val(true));
    canvasB.call<void>("addEventListener", std::string("paste"), val::module_property("clipBoardPaste"), val(true));
    canvasA.call<void>("addEventListener", std::string("copy"), val::module_property("clipBoardCopy"), val(true));
    canvasB.call<void>("addEventListener", std::string("copy"), val::module_property("clipBoardCopy"), val(true));

//    canvasA.call<void>("focus");

    emscripten_set_keydown_callback("canvas-a", nullptr, 0, &keyboard_cb);
    emscripten_set_keyup_callback("canvas-a", nullptr, 0, &keyboard_cb);
    
    enum {
        EventBubblePhase = 0,
        EventCapturePhase = 1
    };
}

static void clipboardCopy(val event)
{
    val target = event["target"];
    std::cout << "copy target " << target["id"].as<std::string>() << std::endl; 
    event["clipboardData"].call<void>("setData", std::string("text/plain"), std::string("TEST TEST TEST"));
    event.call<void>("preventDefault"); // prevent browser from copying currently selected text
}

static void clipboardPaste(val event)
{
    val target = event["target"];
    std::cout << "paste target " << target["id"].as<std::string>() << std::endl;
    val data = event["clipboardData"].call<val>("getData", std::string("text"));
    std::cout << "paste data " << data.as<std::string>() << std::endl;
}

EMSCRIPTEN_BINDINGS(clipboard_module) {
    function("clipBoardCopy", &clipboardCopy);
    function("clipBoardPaste", &clipboardPaste);
}

int main() {
    installEventhandlers();
    emscripten_set_main_loop(noop, 0, 0);
    return 0;
}
