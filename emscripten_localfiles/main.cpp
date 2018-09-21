#include <stdio.h>
#include <functional>

#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

using namespace emscripten;

typedef void (*FileDataCallback)(void *context, char *data, size_t length, const char *name);

void readFileContent(val event)
{
    // Copy file content to WebAssembly memory and call the user file data handler

    val fileReader = event["target"];

    // Set up source typed array
    val result = fileReader["result"]; // ArrayBuffer
    val Uint8Array = val::global("Uint8Array");
    val sourceTypedArray = Uint8Array.new_(result);

    // Allocate and set up destination typed array
    size_t size = result["byteLength"].as<size_t>();
    void *buffer = malloc(size);
    val destinationTypedArray = Uint8Array.new_(val::module_property("HEAPU8")["buffer"], size_t(buffer), size);
    destinationTypedArray.call<void>("set", sourceTypedArray);

    // Call user file data handler
    FileDataCallback fileDataCallback = reinterpret_cast<FileDataCallback>(fileReader["data-filesReadyCallback"].as<size_t>());
    void *context = reinterpret_cast<void *>(fileReader["data-filesReadyCallback"].as<size_t>());
    fileDataCallback(context, static_cast<char *>(buffer), size, fileReader["data-name"].as<std::string>().c_str());
}

void readFiles(val event)
{
    // Read all selcted files using FileReader

    val target = event["target"];
    val files = target["files"];
    int fileCount = files["length"].as<int>();
    for (int i = 0; i < fileCount; i++) {
        val file = files[i];
        val fileReader = val::global("FileReader").new_();
        fileReader.set("onload", val::module_property("qtReadFileContent"));
        fileReader.set("data-filesReadyCallback", target["data-filesReadyCallback"]);
        fileReader.set("data-filesReadyCallbackContext", target["data-filesReadyCallbackContext"]);
        fileReader.set("data-name", file["name"]);
        fileReader.call<void>("readAsArrayBuffer", file);
    }
}

EMSCRIPTEN_BINDINGS(localfileaccess) {
    function("qtReadFiles", &readFiles);
    function("qtReadFileContent", &readFileContent);
};

void loadFile(const char *accept, FileDataCallback callback, void *context)
{
    // Create file input element which will dislay a native file dialog.
    val document = val::global("document");
    val input = document.call<val>("createElement", std::string("input"));
    input.set("type", "file");
    input.set("style", "display:none");
    input.set("accept", val(accept));
    
    // Set JavaScript onchange callback which will be called on file(s) selected,
    // and also forward the user C callback pointers so that the onchange
    // callback can call it. (The onchange callback is actually a C function
    // exposed to JavaScript with EMSCRIPTEN_BINDINGS).
    input.set("onchange", val::module_property("qtReadFiles"));
    input.set("data-filesReadyCallback", val(size_t(callback)));
    input.set("data-filesReadyCallbackContext", val(size_t(context)));
    
    // Programatically activate input
    val body = document["body"];
    body.call<void>("appendChild", input);
    input.call<void>("click");
    body.call<void>("removeChild", input);
}

/*
    Save file by triggering a browser file download.
*/
void saveFile(const char *data, size_t length,  std::wstring fileNameHint)
{
    // Create file data Blob
    val Blob = val::global("Blob");
    val contentArray = val::array();
    val content = val(typed_memory_view(length, data));
    contentArray.call<void>("push", content);
    val type = val::object();
    type.set("type","application/octet-stream");
    val fileBlob = Blob.new_(contentArray, type);

    // Create Blob download link
    val document = val::global("document");
    val link = document.call<val>("createElement", std::string("a"));
    link.set("download", fileNameHint);
    val window = val::global("window");
    val URL = window["URL"];
    link.set("href", URL.call<val>("createObjectURL", fileBlob));
    link.set("style", "display:none");

    // Programatically click link
    val body = document["body"];
    body.call<void>("appendChild", link);
    link.call<void>("click");
    body.call<void>("removeChild", link);
}

// Test harness follows.

bool g_testNoop = false;
extern "C" EMSCRIPTEN_KEEPALIVE void testLoadFile()
{
    if (g_testNoop)
        return;

    void *context = nullptr;
    loadFile("*", [](void *context, char *contentPointer, size_t contentSize, const char *fileName) {
        printf("File loaded %p %d %s\n", contentPointer, contentSize, fileName);
        printf("First bytes: %x %x %x %x\n", contentPointer[0] & 0xff, contentPointer[1] & 0xff,
                                             contentPointer[2] & 0xff, contentPointer[3] & 0xff);
        free(contentPointer);
    }, context);
}

extern "C" EMSCRIPTEN_KEEPALIVE void testSaveFile()
{
    if (g_testNoop)
        return;

    const char *data = "foocontent";
    std::wstring name = L"foofile";
    saveFile(data, strlen(data), name);
}

int main(int argc, char **argv)
{
    // keepalive
    g_testNoop = true;
    testLoadFile();
    testSaveFile();
    g_testNoop = false;
    
    // HTML/JavaScript buttons will call test functions.
}
