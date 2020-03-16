#include <QtCore>

#include <emscripten/bind.h>
#include <emscripten/val.h>

// Export "lerp" function to JavaScript, as per
// https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html
float lerp(float a, float b, float t) {
    return (1 - t) * a + t * b;
}
EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("lerp", &lerp);
}

int main(int argc, char **argv) {

    // We do not block in main. Create the application object on the
    // heap and do not call app.exec(). Combined with -s EXIT_RUNTIME=0
    // this allows for calling the lerp function defined above also
    // after main() returns.
    new QCoreApplication(argc, argv);
    
    // Call JavaScript function from C++, as descripten in
    // https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#using-val-to-transliterate-javascript-to-c
    // Here we access and call the "lerp" function exported above using
    // val::module_property(). It's possible to access other API using val::global().
    emscripten::val lerp = emscripten::val::module_property("lerp");
    float result = lerp(10.0, 20.0, 0.5).as<float>();
    qDebug() << "C++: The result is" << result;

    return 0; 
}