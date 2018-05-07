#include <stdio.h>
#include <functional>

#include <emscripten.h>
#include <emscripten/html5.h>

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

/*
    Save file by triggering a browser file download.

    Note that multiple copies of the file contents may be held in memory at the same time.
*/
void saveFile(const char *contentPointer, size_t contentLength, const char *fileNameHint)
{
    EM_ASM_({
        
        // Make the file contents and file name hint accessible to Javascript
        const contentPointer = $0;
        const contentLength = $1;
        const fileNameHint = Pointer_stringify($2);
        const fileContent = Module.HEAPU8.subarray(contentPointer, contentPointer + contentLength);

        // Create download link and click it programatically
        const fileblob = new Blob([fileContent], { type : 'application/octet-stream' } );
        var link = document.createElement('a');
        document.body.appendChild(link);
        link.download = fileNameHint;
        link.innerHtml = "N/A";
        link.href = window.URL.createObjectURL(fileblob);
        link.style = "display:none";
        link.click();
        document.body.removeChild(link);
    }, contentPointer, contentLength, fileNameHint);
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

extern "C" EMSCRIPTEN_KEEPALIVE void testSaveFile()
{
    if (g_testNoop)
        return;
    const char *data = "foobar";
    saveFile(data, strlen(data), "foobar.txt");
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
