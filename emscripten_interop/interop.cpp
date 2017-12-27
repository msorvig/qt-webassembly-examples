#include <emscripten.h>
#include <emscripten/html5.h>

extern "C" {

// EMSCRIPTEN_KEEPALIVE keeps this function alive to be called from javascript
void EMSCRIPTEN_KEEPALIVE sayHello(int a) {
    printf("hello, world!\n");    
}

void EMSCRIPTEN_KEEPALIVE onMessage(const char *utf8Message) {
    printf("onMessage: %s\n", utf8Message);
}

}

// Inject javascript which calls C functions
void callSayHello()
{
    EM_ASM(
        Module.ccall('sayHello');
    );

    EM_ASM(
        Module.ccall('sayHelloFromArchive');
    );
}

void injectStringConvertFunctions()
{
    EM_ASM(
        if (Module.qt_toUtf8Allocate != null)
            return;
        
        // Convenience string convertion function. Remeber to
        // free the returned buffer.
        function qt_toUtf8Allocate(javascriptString)
        {
            // Allocate buffer with orst case UTF8 size. This is 
            // okay since this a temporary buffer.
            var utf8length = javascriptString.length * 4 * 1;
            var utf8Buffer = Module._malloc(utf8length);
            Module.stringToUTF8(javascriptString, utf8Buffer, utf8length);
            return utf8Buffer;
        }

        // Attach function to gloabal Module object
        Module.qt_toUtf8Allocate = qt_toUtf8Allocate;

        // Maybe declare matching free and stringify
        Module.qt_toUtf8Free = Module._free;
        Module.qt_stringify = Module.Pointer_stringify;
    );
}

// string to javascript and back
void sendStringMessage()
{
    const char *message = "Hello via Javascript: ٩(͡๏̯͡๏)۶";

    EM_ASM_({
        var helloString = Module.qt_stringify($0);

        var utf8Buffer = Module.qt_toUtf8Allocate(helloString);

        Module.ccall('onMessage', null, ['number'], [utf8Buffer]);

        Module.qt_toUtf8Free(utf8Buffer);
    }, message);
}

void printStackTrace()
{
    EM_ASM(
        console.log(stackTrace());
    );
}

bool g_keepAliveCall;

extern "C" void sayHelloFromArchive();

EMSCRIPTEN_KEEPALIVE int main() {
    // Functions that are called from javascript only may be removed by the linker.
    // Force them to be inlcuded by calling them here with a "no-op" flag set.
    g_keepAliveCall = true;
    sayHelloFromArchive();
    g_keepAliveCall = false;

    callSayHello();
//    printStackTrace();
    
    injectStringConvertFunctions();
    sendStringMessage();
    
}