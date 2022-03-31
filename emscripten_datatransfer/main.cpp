
#include <stdio.h>
#include <iostream>

#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

// Copy data from a JS buffer to the C heap. Caller owns the returned buffer.
unsigned char *copyToHeap(emscripten::val arrayBuffer)
{
    const int size = arrayBuffer["byteLength"].as<int>();

    // Allocate a destination buffer and make that buffer visible to
    // JavaScript via typed_memory_view(), which creates a Uint8Array
    // object.
    unsigned char *data = (unsigned char *)malloc(size);
    emscripten::val dst = emscripten::val(emscripten::typed_memory_view(size, data));

    // Copy data using using Uint8Array::set().
    auto src = emscripten::val::global("Uint8Array").new_(arrayBuffer);
    dst.call<void>("set", src);
    return data;
}

void processBuffer(emscripten::val arrayBuffer)
{
    unsigned char *data = copyToHeap(arrayBuffer);
    std::cout << "content " << int(data[0]) << " " << int(data[1])<<  " "  << int(data[2]) << std::endl;
    free(data);
}

EMSCRIPTEN_BINDINGS(datatransfer) {
    emscripten::function("processBuffer", &processBuffer);
}

int main() {
    return 0;
}
