#include <stdio.h>
#include <iostream>

#include <emscripten.h>
#include <emscripten/bind.h>

#include "qstdweb_gui.h"

using namespace std;
using namespace emscripten;
using namespace qstdweb;

void setup() {
    Element container = Document::global().getElementById("container");

    // Use the absolute-relative postitioning trick to position child elements
    // at a given (x,y) offset on the container.
    container.setStylePosition("relative");
    container.setStyleLeftTop(0, 0);

    // Add all input element types

    auto inputTypes = { "button",  "checkbox",  "color",  "date" "datetime-local",  "email", "file",
                        "hidden",  "image",  "month", "number", "password", "radio", "range",
                        "reset", "search", "submit", "tel", "text", "time", "url", "week" };

    int y = 0;
    for (auto inputType : inputTypes) {
        DivElement label;
        label.appendChild(TextNode(inputType));
        container.appendChild(label);
        label.setStylePosition("absolute");
        label.setStyleLeftTop(10, y);

        InputElement input = InputElement(inputType);
        container.appendChild(input);
        input.setStylePosition("absolute");
        input.setStyleLeftTop(150, y);

        if (std::string(inputType) == std::string("text")) {
            auto keyDownHandler = [](const Event& event) {
                std::cout << "keydown" << std::endl;
                //std::string key = event["key"].as<std::string>();
            };
            input.addEventListender("keydown", keyDownHandler);
            auto blurHandler = [](const Event& event) {
                std::cout << "blur" << std::endl;
            };
            input.addEventListender("blur", blurHandler);
        }

        y += 50;
    }

    // Add a video element
    {
        DivElement label;
        label.appendChild(TextNode("video (BigBuckBunny.mp4)"));
        container.appendChild(label);
        label.setStylePosition("absolute");
        label.setStyleLeftTop(0, y);

        VideoElement video("http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4");
        container.appendChild(video);
        video.setStylePosition("absolute");
        video.setStyleLeftTop(200, y);
        video.setStyleWidthHeight(320, 200);
        video.set("controls", std::string(" "));
        video.set("currentTime", 20);
        y+=250;
    }

    // Add iframe
    {
        DivElement label;
        label.appendChild(TextNode("iframe (example.com)"));
        container.appendChild(label);
        label.setStylePosition("absolute");
        label.setStyleLeftTop(0, y);

        IframeElement iframe("https://example.com");
        container.appendChild(iframe);
        iframe.setStylePosition("absolute");
        iframe.setStyleLeftTop(200, y);
        iframe.setStyleWidthHeight(320, 200);
        iframe["style"].set("border", std::string("0"));
    }
}

EMSCRIPTEN_BINDINGS(my_module) {
//    emscripten::function("setup", setup);
}

int main() {
    setup();
    return 0;
}
