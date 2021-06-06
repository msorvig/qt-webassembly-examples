# Qt application main() on WebAssembly with Emscripten


**Note: This is a work in progress, and may describe behavior of a future version of Qt 6.**

## Examples Overview

These examples demonstrate different approaches for implementing the main()
function when targeting WebAssembly, as well as related concepts.

    javascript_onload             Intro: You can have a "main()" with on the web, but it can't block
    qt_main_exec                  Standard Qt main with app.exec(), where exec() never returns
    qt_main_noexec                New-style main() without exec() call
    qt_main_noexec_multi          Detroying and re-creating the application object
    emscripten_exit_runtime       Using Emscripten's EXIT_RUNTIME feature

## The problem with main() on the web

The Web, WebAssembly, and Emscripten are primarily callback-based. This means that the application should
not block the main thread for extended periods of time; doing so may cause the browser to determine that
the page (and application) has become non-responsive and prompt the user to close the tab.

For example, a web app might install an onload event handler in order to run code at startup:

```
function main() {
    console.log(“Hello”);
    // control returns to the browser here
}
<body onload=“main”>
```
*(See javascript_onload/)*

Qt applications use a different style, where control stays in main() for the lifetime of the program:

```
int main(int argc, char **argv)
{
    QApplication app = QApplication(argc, argv);
    AppWindow window = AppWindow();
    return app.exec(); // <- control stays here for the app lifetime.
}
```
This is not compatible with web callbacks; as discussed above the browser will determine that the
page has become unresponsive.

## Solutions for main() on the web

Qt provides the application with several options:

1) Keep the standard main() implementation; Qt applies workardound hack from Emscripten
2) Rewrite main() to avoid exec() (preferred)
3) Use Emscriptens asyncify feature
4) (Run Qt on a secondary thread / web worker) (out of scope)

### Keep the standard main() implementation

This works as expected, except that the exec() call never returns:

```
int main(int argc, char **argv)
{
    QApplication app = QApplication(argc, argv);
    QWindow window = AppWindow();
    int exitCode = app.exec();

    // ### Control never reaches this point; cleanup code does not run.

    return code;
}
```
*(See qt_main_exec/)*

This is implemented by calling Emscripten API which transfers control back to the browser without
unwinding the C++ stack. (Specifically, the API is emscripten_set_main_loop() with simulate_infinite_loop
set to true). This has the side effects that allocations made before the exec() call are not free'd,
and that cleanup code does not run.

The app runs until the browser terminates the process. QApplication exit handles and atexit()
handlers are not called.

### Change the main() implementation

The easiest way to avoid blocking in main is to not call QApplication::exec(). This typically
requires rewriting the main function to construct the root application objects on the heap
instead of on the stack.

```
QWindow *g_window;
int main(int argc, char **argv)
{
    new QApplication(argc, argv);
    g_window = new AppWindow();

    // control exits here, but app does not quit
}
```
*(See qt_main_noexec/ and qt_main_noexec_multi/)*

This gives user code control over Qt resource usage; deleting the application object will
shut down the Qt runtime. The app can do this in response to page lifecycle events, see
“Handling page lifecycle” below [TODO].

### Use Asyncify:

Emscripten’s asyncify feature supports blocking API like QApplication::exec(), however it
supports only one level of blocking calls. We recommend reserving this for other uses, such
as showing modal dialogs. See the asyncify section below [TODO].


## EXIT_RUNTIME

By default, Emscripten does not shut down the C++ runtime when main() exits, which means that the app
can continue to run. It is possible to change this behavior by passing the "-s EXIT_RUNTIME=1" flag
on the em++ linker line.

However, this does not appear to work well when calling emscripten_pause_main_loop() or emscripten_cancel_main_loop()
(Qt uses these to pause updates): the updates are paused, but the runtime is exited as well (global destructors run).
See emscripten_exit_runtime/.

Qt 5 sets -s EXIT_RUNTIME=1. Qt 6 does not.

## QDialog::exec()

QDialog::exec() works only partially on Qt for WebAssambly. The dialog will be shown, the user can interact with
it and the appropriate signals will be emitted on close.

However, the exec() call never returns, stack content at the time of the exec() call is leaked, and the exec() call
may interfere with event processing of the current event.

Based on this, QDialog::exec() is not supported and we reccomend using QDialog::show() instead.

See the asyncify section below [TODO].

## Asyncify
[TODO]
