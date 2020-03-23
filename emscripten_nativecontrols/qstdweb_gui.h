
#ifndef QSTDWEB_GUI_H
#define QSTDWEB_GUI_H

#include "qstdweb_core.h"

#include <tuple>
#include <string_view>

namespace qstdweb {

    class Element;
    class TextNode;

    class Node : public emscripten::val {
    public:
        Node();
        explicit Node(const emscripten::val &inner);
        emscripten::val inner();
    };

    class Document: public Node {
    public:
        static Document global();
        explicit  Document(const emscripten::val &inner);
        Element createElement(std::string_view tagName);
        TextNode createTextNode(std::string_view text);
        Element getElementById(std::string_view id);
    };

    class EventListener
    {
    private:
        EventListener(EventCallback *eventCallbabck);
        EventCallback *m_callback = nullptr;
        friend class Element;
    };

    class Event : public Node
    {
    public:
        Event();
        explicit Event(const emscripten::val &inner);
    };

    class TextNode : public Node {
    public:
        explicit TextNode(const emscripten::val &inner);
        explicit TextNode(std::string_view text);
    };

    class Element : public Node {
    public:
        Element();
        explicit Element(const emscripten::val &inner);
        explicit Element(std::string_view tagName);

        void appendChild(Node child);
        void removeChild(Node child);

        void setParent(const Element &parent);
        Element parent() const;

        EventListener addEventListender(std::string_view event,
            std::function<void (const Event& event)> callback);
        void removeEventListener(EventListener eventListener);

        void setStylePosition(std::string_view position);
        std::string stylePosition() const;

        void setStyleLeftTop(float x, float y);
        std::tuple<float, float> styleLeftTop() const;

        void setStyleWidthHeight(float width, float height);
        std::tuple<float, float> styleWidthHeight() const;
    };

    class InputElement : public Element {
    public:
        InputElement(std::string_view type);

        void setType(std::string_view type);
    };

    class DivElement : public Element {
    public:
        DivElement();
    };

    class PasswordInputElement : public InputElement {
    public:
        PasswordInputElement();

        enum Autocomplete {
            CurrentPassword,
            NewPassword
        };
        void setAutocomplete(Autocomplete autoComplete);
        Autocomplete autocomplete();

        void setMaxlength(int maxLength);
        int maxLength() const;
        void setMinlength(int maxLength);
        int minLength() const;
        void setPattern(std::string_view pattern);
        std::string pattern() const;
        void setPlaceholder(std::string_view placeholder);
        std::string placeholder();
        void setReadonly(bool readOnly);
        bool readonly() const;
        void setSize(int characterSize);
        int size() const;
    };

    class VideoElement : public Element {
    public:
        VideoElement();
        VideoElement(std::string_view url);
    };

    class IframeElement : public Element {
    public:
        IframeElement(std::string_view url);
    };
}

#endif
