Qt WebAssembly Examples
=======================

This repository contains examples and test cases for Qt on WebAssenbly.

Qt for WebAssembly makes it possible to run Qt applications on many
web browsers, without any special server requirements (other than serving
the wasm file). See [qt-webassembly-examples](https://msorvig.github.io/qt-webassembly-examples/)
for live demos.

[QTBUG-63917](https://bugreports.qt.io/browse/QTBUG-63917) contains
further information on the Qt for WebAssembly port.

Note on cloning this repository: The gh-pages branch contains the example
binaries. Clone with --single-branch to minimize download size.

    git clone -b master --single-branch git@github.com:msorvig/qt-webassembly-examples.git

Example Categories:

    html_          html behavior test cases (no Qt usage)
    emscripten_    emscripten behavior test cases (no Qt usage)
    gui_           Qt Gui examples
    widgets_       Qt Widgets examples
    quick_         Qt Quick examples