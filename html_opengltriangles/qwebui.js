function qwebui_button(parentElement, text, onClicked) {
    let button = document.createElement("button");
    button.style.margin = "5px"
    button.appendChild(document.createTextNode(text));
    parentElement.appendChild(button);
    button.addEventListener("click", onClicked);
    return {
        setEnabled : (enabled) => { button.disabled = !enabled }
    }
}

function qwebui_checkbox(parentElement, text, inital, onToggled) {
    let id = "qwebui_checkbox_" + text; // well...
    let checkbox = document.createElement('input');
    checkbox.style.margin = "5px"
    checkbox.type = "checkbox";
    checkbox.name = "name";
    checkbox.value = "value";
    checkbox.id = id;
    checkbox.checked = inital;
    checkbox.addEventListener('change', (event) =>  { onToggled(checkbox.checked); });
    parentElement.appendChild(checkbox);
    let label = document.createElement('label')
    label.htmlFor = id;
    label.appendChild(document.createTextNode(text));
    parentElement.appendChild(label);
}

function qwebui_label(parentElement, text) {
    let label = document.createElement('label')
    let textNode = document.createTextNode(text);
    label.appendChild(textNode);
    label.style.margin = "5px"
    parentElement.appendChild(label);
    return {
        setText : (text) => { textNode.nodeValue = text; }
    }
}

