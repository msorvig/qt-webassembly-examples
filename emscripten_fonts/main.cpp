#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

#include <iostream>

using namespace emscripten;

int main(int argc, char **argv)
{
    // A brief emscripten::val intro. emscripten::val is the preferred way for accessing native API 
    // when implementing Qt for WebAssembly, in cases where Emscripten does not provide a C/C++ wrapper
    // API.
    
    // See reference documentation at https://emscripten.org/docs/api_reference/val.h.html

    // emscripten::val allows us to call Web platform API from C++ without writing JavaScript
    // code. This has the benefit that the Qt implementation can use aa single language, that
    // we avoid C++/JS interop issues, and that we avoid having to deploy JS code with Qt.
    // It is expected that the val API (or an API like it) will be made more peformant once
    // implemented using wasm reference types, which reduce or elliminte the need for JS glue code.

    // (The folllowing uses the Chrome local fonts API as a test case, which you might have to
    // enable in chrome://settings)

    // -- Objects and properties:
    // You can look up a global objects using emscripten::val::global(), and access properties
    // using operator[]. Use MDN to look up native API: https://developer.mozilla.org/en-US/docs/Web/API/Navigator
    val navigator = val::global("navigator");
    val fonts = navigator["fonts"];

    // -- isUndefined() and isNull() can be used to check if an object is valid
    std::cout << "fonts undefined? " << fonts.isUndefined() << " null? " << fonts.isNull() << std::endl;

    if (fonts.isUndefined())
        return 0;
    
    // Use typeof() to query the object type; which often will be "object". (Make sure to build
    // in in e.g. -std=c++17 mode in order to disable the "typeof" compiler extention)
    std::cout << "fonts typeof: " << fonts.typeof().as<std::string>() << std::endl;

    // Use the constructor.name property to further inspect what kind of object you have
    std::cout << "constructor name: " << fonts["constructor"]["name"].as<std::string>() << std::endl;

    // -- Query for font access permission. This involves making a function call to the
    // Permissions::query native API, and then handling the returned JS Promise using callbacks.
    val permissions = navigator["permissions"];

    val queryParams = val::object(); // create new JS object, see also val::array()
    queryParams.set("name", std::string("font-access")); // the val API can be peculiar on const char * vs std::string usage

    // Make a function call using the call<return_type>(function_name, arg1, arg2, ...)  API
    val statusPromise = permissions.call<val>("query", queryParams);

    // Connect success and error promise handlers. The handler functions are exported using
    // EMSCRIPTEN_BINDINGS below and we us module_property() to get a val handle to them.
    statusPromise.call<void>("then", val::module_property("onFontPermissionSuccess"));
    statusPromise.call<void>("catch", val::module_property("onFontPermissionError"));

    // TODO: access the local fonts using navigator.fonts.query();

    return 0;
}

void onFontPermissionSuccess(val status)
{
    // Print status, using val::as() to convert the status.state val to a C++ type
    std::cout << "onFontPermissionSuccess: " << status["state"].as<std::string>() << std::endl;
}

void onFontPermissionError(val error)
{
    std::cout << "onFontPermissionError" << std::endl;
}

EMSCRIPTEN_BINDINGS(qtFonts) {
    emscripten::function("onFontPermissionSuccess", onFontPermissionSuccess);
    emscripten::function("onFontPermissionError", onFontPermissionError);
}

