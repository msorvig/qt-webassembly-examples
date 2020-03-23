
#include "qstdweb_gui.h"

#include <iostream>
#include <string>

namespace qstdweb {

Node::Node()
:emscripten::val(emscripten::val::undefined())
{

}

Node::Node(const emscripten::val &inner)
:emscripten::val(inner)
{

}

emscripten::val Node::inner()
{
    return *this;
}

Document Document::global()
{
    return Document(emscripten::val::global("document"));
}

Document::Document(const emscripten::val &inner)
:Node(inner)
{

}

Element Document::createElement(std::string_view tagName)
{
    return Element(call<emscripten::val>("createElement", std::string(tagName)));
}

TextNode Document::createTextNode(std::string_view text)
{
    return TextNode(call<emscripten::val>("createTextNode", std::string(text)));
}
Element Document::getElementById(std::string_view id)
{
    return Element(call<emscripten::val>("getElementById", std::string(id)));
}

EventListener::EventListener(EventCallback *eventCallbabck)
:m_callback(eventCallbabck)
{

}

Event::Event()
:Node()
{

}

Event::Event(const emscripten::val &inner)
:Node(inner)
{

}

TextNode::TextNode(const emscripten::val &inner)
:Node(inner)
{

}

TextNode::TextNode(std::string_view text)
:Node(Document::global().createTextNode(text))
{

}

Element::Element()
:Node()
{

}

Element::Element(const emscripten::val &inner)
:Node(inner)
{

}

Element::Element(std::string_view tagName)
:Node(Document::global().createElement(tagName))
{

}

void Element::appendChild(Node child)
{
    call<void>("appendChild", child.inner());
}

EventListener Element::addEventListender(std::string_view event,
    std::function<void (const Event& event)> callback)
{
    auto fn = [callback](const emscripten::val &event){ callback(Event(event));};
    EventCallback *cb = new EventCallback(*this, std::string(event), fn);
    return EventListener(cb);
}

void Element::removeEventListener(EventListener eventListener)
{
    delete eventListener.m_callback;
    eventListener.m_callback = nullptr;
}

void Element::setStylePosition(std::string_view position)
{
    (*this)["style"].set("position", std::string(position));
}

std::string Element::stylePosition() const
{
    return (*this)["style"]["position"].as<std::string>();
}

void Element::setStyleLeftTop(float x, float y)
{
    (*this)["style"].set("left", std::to_string(x) + std::string("px"));
    (*this)["style"].set("top", std::to_string(y) + std::string("px"));
}

std::tuple<float, float> Element::styleLeftTop() const
{
    return std::make_tuple((*this)["style"]["left"].as<float>(),
                           (*this)["style"]["top"].as<float>());
}

void Element::setStyleWidthHeight(float width, float height)
{
    (*this)["style"].set("width", std::to_string(width) + std::string("px"));
    (*this)["style"].set("height", std::to_string(height) + std::string("px"));
}

std::tuple<float, float>  Element::styleWidthHeight() const
{
    return std::make_tuple((*this)["style"]["width"].as<float>(),
                           (*this)["style"]["height"].as<float>());
}

InputElement::InputElement(std::string_view type)
:Element("input")
{
    setType(type);
}

void InputElement::setType(std::string_view type)
{
    set("type", std::string(type));
}

DivElement::DivElement()
:Element("div")
{

}

VideoElement::VideoElement()
{

}

VideoElement::VideoElement(std::string_view url)
:Element("video")
{
    // short-cut one source element
    Element source = Document::global().createElement("source");
    source.set("src", std::string(url));
    appendChild(source);
}

IframeElement::IframeElement(std::string_view url)
:Element("iframe")
{
    set("src", std::string(url));
}

} // namepsace qstdweb
