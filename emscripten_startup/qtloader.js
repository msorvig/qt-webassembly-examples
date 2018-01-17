//
// QtLoader
//
// QtLoader provides convenience javascript API for managing Qt application modules.
//
// QtLoader provides API on top of Emscripten which supports common lifecycle
// tasks such as displaying placeholder content while the module downloads,
// handing application exits, and checking for browser wasm support.
//
// Basic usage: 
// 
//     var config = {
//         containerElements : [$("container-id")];
//     }
//     var qtLoader = QtLoader(config);
//     qtLoader.loadEmscriptenModule(Module);
//
// Config keys
//
//  containerElements : [container-element, ...]
//      One or more HTML elements. QtLoader will display loader elements
//      on these while loading the applicaton, and replace the loader with a 
//      canvas on load complete.
//  showLoader : function(containerElement)
//      Optional loading element constructor function. Implement to create
//      a custom loading "screen".
//  showCanvas : function(containerElement)
//      Optional canvas constructor function. Implement to create custom
//      canvas elements.
//  showExit : function(containerElement, crashed, exitCode)
//      Optional exited element constructor function.
//  showError : function(containerElement, crashed, exitCode)
//      Optional error element constructor function.
//
//  path : <string>
//      Prefix path for wasm file, realative to the loading HMTL file.
//  restartMode : "DoNotRestart", "RestartOnExit", "RestartOnCrash"
//      Controls whether the application should be reloaded on exits. The default is "DoNotRestart"
//  restartLimit : <int>
//     Restart attempts limit. The default is 10.
//  stdoutEnabled : <bool>
//  stderrEnabled : <bool>
//
//
// QtLoader object API
//
// webAssemblySupported : bool
// webGLSupported : bool
// canLoadQt : bool
//      Reports if WebAssembly and WebGL are supported. These are requirements for
//      running Qt applications.
// loadEmscriptenModule(createModule)
//      Loads the applicaton from the given emscripten module create function
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
    function webAssemblySupported() {
        return typeof WebAssembly !== undefined
    }
    
    
    function webGLSupported() {
        // We expect that WebGL is supported if WebAssembly is; however
        // the GPU may be blacklisted.
    	try {
    		var canvas = document.createElement("canvas");
            return !!(window.WebGLRenderingContext && canvas.getContext("webgl"));
    	} catch (e) {
    		return false;
    	}
    }
    
    function canLoadQt() {
        // The current Qt implementation requires WebAssembly (asm.js is not in use),
        // and also WebGL (there is no raster fallback).
        return webAssemblySupported() && webGLSupported();
    }
    
    function removeChildren(element) {
        while (element.firstChild) element.removeChild(element.firstChild);        
    }

    // Set default state handler functions if needed
    config.showError = config.showError || function(container, errorText) {
        removeChildren(container);
        var errorTextElement = document.createElement("text");
        errorTextElement.className = "QtError"
        errorTextElement.innerHTML = errorText;
        return errorTextElement;
    }
    config.showLoader = config.showLoader || function(container) {
        removeChildren(container);
        var loadingText = document.createElement("text");
        loadingText.className = "QtLoading"
        loadingText.innerHTML = "<p><center>Loading Qt ...</center><p>";
        return loadingText;
    };
    config.showCanvas = config.showCanvas || function(container) {
        removeChildren(container);
        var canvas = document.createElement("canvas");
        canvas.className = "QtCanvas"
        canvas.style = "height: 100%; width: 100%;"
        return canvas;
    }
    config.showExit = config.showExit || function(containerElement, crashed, exitCode) {
        if (!crashed)
            return undefined;
        removeChildren(containerElement);
        var fontSize = 54;
        var crashSymbols = ["\u{1F615}", "\u{1F614}", "\u{1F644}", "\u{1F928}", "\u{1F62C}", 
                            "\u{1F915}", "\u{2639}", "\u{1F62E}", "\u{1F61E}", "\u{1F633}"];
        var symbolIndex = Math.floor(Math.random() * crashSymbols.length);
        var errorHtml = `<font size='${fontSize}'> ${crashSymbols[symbolIndex]} </font>`
        var errorElement = document.createElement("text");
        errorElement.className = "QtExit"
        errorElement.innerHTML = errorHtml;
        return errorText;
    }
    
    config.restartMode = config.restartMode || "DoNotRestart";
    config.restartLimit = config.restartLimit || 10;

    if (config.stdoutEnabled === undefined) config.stdoutEnabled = true;
    if (config.stderrEnabled === undefined) config.stderrEnabled = true;

    // Make sure config.path is defined and ends with "/" if needed
    if (config.path === undefined)
        config.path = "";
    if (config.path.length > 0 && !config.path.endsWith("/"))
        config.path = config.path.concat("/");
   
    var publicAPI = {};
    publicAPI.webAssemblySupported = webAssemblySupported();
    publicAPI.webGLSupported = webGLSupported();
    publicAPI.canLoadQt = canLoadQt();
    publicAPI.canLoadApplication = canLoadQt();
    publicAPI.status = undefined;
    publicAPI.crashed = false;
    publicAPI.exitCode = undefined;
    publicAPI.loadEmscriptenModule = loadEmscriptenModule;
    
    restartCount = 0;
    
    function loadEmscriptenModule(createModule) {
        
        // Check for Wasm & WebGL support; return early before
        if (!webAssemblySupported()) {
            self.error = "Error: WebAssembly is not supported"
            setStatus("Error");
            return;
        }
        if (!webGLSupported()) {
            self.error = "Error: WebGL is not supported"
            setStatus("Error");
            return;
        }

        // Create module object for customization
        var module = {};
        self.module = module;
        
        module.locateFile = module.locateFile || function(filename) {
            return config.path + filename;
        }

        // Attach status callbacks
        module.setStatus = module.setStatus || function(text) {
            // Currently the only usable status update from this function
            // is "Running..."
            if (text.startsWith("Running"))
                setStatus("Running");
        }
        module.monitorRunDependencies = module.monitorRunDependencies || function(left) {
          //  console.log("monitorRunDependencies " + left)
        }

        // Attach standard out/err callbacks.
        module.print = module.print || function(text) {
            if (config.stdoutEnabled)
                console.log(text)
        }
        module.printErr = module.printErr || function(text) {
            if (config.stdErr)
                console.log(text)
        }

        // Error handling: set status to "Exited", update crashed and
        // exitCode according to exit type.
        // Emscrjpten will typically call printErr with the error text
        // as well. Note that emscripten may also throw exceptions from
        // async callbacks. These should be handled in window.onerror by user code.
        module.onAbort = module.onAbort || function(text) {
            publicAPI.crashed = true;
            setStatus("Exited");
        }
        module.quit = module.quit || function(code, exception) {
            if (exception.name == "RangeError") {
                // Emscripten calls module.quit with RangeError on stack owerflow
                publicAPI.crashed = true;
            } else {
                // Clean exit with code
                publicAPI.exitCode = code;
            }

            setStatus("Exited");
        }
        
        config.restart = function() {
            ++self.restartCount;
            if (self.restartCount > config.restartLimit) {
                self.error = "Error: This application has crashed too many times and has been disabled. Reload the page to try again."
                setStatus("Error");
                return;
            }
            loadEmscriptenModule(createModule);
        } 

        publicAPI.exitCode = undefined;
        publicAPI.crashed = false;
        setStatus("Loading");
        
        // Finall call emscripten create with our config object
        createModule(module);
    }

    function setErrorContent() {
        for (container of config.containerElements) {
            var errorElement = config.showError(container, self.error);
            container.appendChild(errorElement);
        }
    }

    function setLoaderContent() {
        for (container of config.containerElements) {
            var loaderElement = config.showLoader(container);
            container.appendChild(loaderElement);
        }
    }
    
    function setCanvasContent() {
        for (container of config.containerElements) {
            var loaderElement = config.showCanvas(container);
            container.appendChild(loaderElement);
        }
        if (self.module.canvas === undefined) {
            var firstCanvas = config.containerElements[0].firstChild;
            self.module.canvas = firstCanvas;
        }
    }
    
    function setExitContent() {
        
//        publicAPI.crashed = true;
        
        if (publicAPI.status != "Exited")
            return;

        // Leave canvas content on clean exit
        if (!publicAPI.crashed)
            return;
        
        for (container of config.containerElements) {
            var loaderElement = config.showExit(container, publicAPI.crashed, publicAPI.exitCode);
            container.appendChild(loaderElement);
        }
    }
    
    var committedStatus = undefined;
    function handleStatusChange() {
        if (committedStatus == publicAPI.status)
            return;
        committedStatus = publicAPI.status;

        if (publicAPI.status == "Exited") {
            if (config.restartMode == "RestartOnExit" ||
                config.restartMode == "RestartOnCrash" && !publicAPI.crashed) {
                    config.restart();
            } else {
                setExitContent();
            }
        }
    }
    
    function setStatus(status) {
        if (publicAPI.status == status)
            return;
        publicAPI.status = status;
        
        if (publicAPI.status == "Error") {
            setErrorContent();
        } else if (publicAPI.status == "Loading") {
            setLoaderContent();
        } else if (publicAPI.status == "Running") {
            setCanvasContent();
        }
        
        // Send status change notification
        if (config.statusChanged)
            config.statusChanged(status);
        
        // handle exit status change 'later'
        window.setTimeout(function() { handleStatusChange(); }, 0);
    }

    setStatus("Created");

    return publicAPI;
}
