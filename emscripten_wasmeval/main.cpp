
// Prototypes for javascript eval functions
extern "C" void js_eval(const char *codestring);
extern "C" int js_eval_int(const char *codestring);
extern "C" double js_eval_double(const char *codestring);


// usage example
extern "C" int fortytwo() {

    // evaluate javascript from C++
    js_eval("console.log(\"hello from javascript 1\")");
    js_eval("console.log(\"hello from javascript 2\")");
    js_eval_double("42.3");

    return js_eval_int("42");
}
