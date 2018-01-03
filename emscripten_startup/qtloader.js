//
// QtLoader
//
// Config keys
//
//  canvasElements : [canvas-element, ...]
//      One or more HTML canvas elements. Qt Will map these to QScreen(s).
//  containerElements : [container-element, ...]
//      One or more HTML div elements. QtLoader will display spinner elements
//      on these while loading, and then replace the spinner with a canvas
//      on load complete.
//  spinnerElementCreate : function(containerElement)
//      A custom HMTL element to be used as placeholder content while the
//      application is loading. Used with containerElements.
//  module: {}
//      An emscripten module config.
// statusChanged : "Created", "Loading", "Running", "Exited"
//
// API
//
// loadEmscriptenModule(createModule)
//      Loads the applicaton from the given module create function
// status
//      One of "Created", "Loading", "Running", "Exited".
// crashed
//      Set to true if there was an unclean exit.
// exitCode
//      main()/emscripten_force_exit() return code. Valid on status change to
//      "Exited", iff crashed is false.
//
function QtLoader(config)
{
    function loadEmscriptenModule(createModule) {
        publicAPI.exitCode = undefined;
        publicAPI.crashed = false;

        // Create module object for customization
        var module = config.module || {};

        // Attach status and standard out/err callbacks.
        module.setStatus = module.setStatus || function(text) {
            // console.log("QtLoader.setStatus '" + text + "'")
            if (text.startsWith("Running"))
                updateStatus("Running");
        }
        module.print = module.print || function(text) {
            console.log("QtLoader.print " + text)
        }
        module.printErr = module.printErr || function(text) {
            console.log("QtLoader.printErr " + text)
        }

        // Error handling: set status to "Exited" and crashed to true.
        // Emscrjpten will typically call printErr with the error text
        // as well. Note that emscripten may also throw exceptions from
        // async callbacks. These should be handled in window.onerror by user code.
        module.onAbort = module.onAbort || function(text) {
            publicAPI.crashed = true;
            updateStatus("Exited");
        }
        module.quit = module.quit || function(code, exception) {
            if (exception.name == "RangeError") {
                // Emscripten calls module.quit with RangeError on stack owerflow
                publicAPI.crashed = true;
            } else {
                // Clean exit with code
                publicAPI.exitCode = code;
            }
            updateStatus("Exited");
        }

        // Set canvas: the first canvas element becomes the emscripten default canvas
        // and the first Qt screen.
        module.canvas = config.canvasElements[0];

        updateStatus("Loading");
        createModule(module);
    }

    function updateStatus(status) {
        if (!config.statusChanged)
            return;
        if (publicAPI.status == status)
            return;
        publicAPI.status = status;
        config.statusChanged(status);
    }

    var publicAPI = {
        "loadEmscriptenModule" : loadEmscriptenModule,
        "status" : "",
        "crashed" : false,
        "exitCode" : undefined
    };

    updateStatus("Created");

    return publicAPI;
}
