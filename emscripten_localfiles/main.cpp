#include <stdio.h>
#include <functional>

#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>

std::function<void(char *, size_t, const char *)> g_qtFileDataReadyCallback;
extern "C" EMSCRIPTEN_KEEPALIVE void qt_callFileDataReady(char *content, size_t contentSize, const char *fileName)
{
    if (g_qtFileDataReadyCallback == nullptr)
        return;

    g_qtFileDataReadyCallback(content, contentSize, fileName);
    g_qtFileDataReadyCallback = nullptr;
}

// C++ local file access API
void loadFile(const char *accept, std::function<void(char *, size_t, const char *)> fileDataReady)
{
    if (g_qtFileDataReadyCallback)
        puts("Warning: Concurrent loadFile() calls are not supported. Cancelling earlier call");

    // Call qt_callFileDataReady to make sure the emscripten linker does not 
    // optimize it away. Set g_qtFileDataReadyCallback to null to make it a a no-op.
    g_qtFileDataReadyCallback = nullptr;
    qt_callFileDataReady(nullptr, 0, nullptr); 
                            
    g_qtFileDataReadyCallback = fileDataReady;
    
    EM_ASM_({

        const accept = Pointer_stringify($0);
        
        // Crate file file input which whil display the native file dialog
        var fileElement = document.createElement("input");
        document.body.appendChild(fileElement);
        fileElement.type = "file";
        fileElement.style = "display:none";
        fileElement.accept = accept;
        fileElement.onchange = function(event) {
            const files = event.target.files;

            // Read files
            for (var i = 0; i < files.length; i++) {
                const file = files[i];
                var reader = new FileReader();
                reader.onload = function() {
                    const name = file.name;
                    var contentArray = new Uint8Array(reader.result);
                    const contentSize = reader.result.byteLength;
                    
                    // Copy the file file content to the C++ heap.
                    // Note: this could be simplified by passing the content as an
                    // "array" type to ccall and then let it copy to C++ memory.
                    // However, this built-in solution does not handle files larger
                    // than ~15M (Chrome). Instead, allocate memory manually and
                    // pass a pointer to the C++ side (which will free() it when done).
                    const heapPointer = _malloc(contentSize);
                    const heapBytes = new Uint8Array(Module.HEAPU8.buffer, heapPointer, contentSize);
                    heapBytes.set(contentArray);
                    
                    // Null out the first data copy to enable GC. TODO: read file in chunks to avoid
                    // holding two copies in memory here.
                    reader = null;
                    contentArray = null;
                    
                    // Call the C++ file data ready callback
                    ccall("qt_callFileDataReady", null, 
                        ["number", "number", "string"], [heapPointer, contentSize, name]);
                };
                reader.readAsArrayBuffer(file);
            }
            
            // Clean up document
            document.body.removeChild(fileElement);

        }; // onchange callback

        // Trigger file dialog open
        fileElement.click();
    }, accept);
}

// Test harness follows.
bool g_testNoop = false;
extern "C" EMSCRIPTEN_KEEPALIVE void testLoadFile()
{
    if (g_testNoop)
        return;
    loadFile("*", [](char *contentPointer, size_t contentSize, const char *fileName){
        printf("File loaded %p %d %s\n", contentPointer, contentSize, fileName);
        printf("First bytes: %x %x %x %x\n", contentPointer[0] & 0xff, contentPointer[1] & 0xff, 
                                             contentPointer[2] & 0xff, contentPointer[3] & 0xff);
        free(contentPointer);
    });
}

using namespace emscripten;

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
