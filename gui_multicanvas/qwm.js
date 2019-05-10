// A minimal html window manager
let windows = []
let root = document.body;
let windowWidth = 300;
let windowheight = 200;
let resizeHandleSize = 15
var currentMaxZ = 0;

class Window {
    constructor(x, y) {

        // Window div structure
        // windowContaine
        // |-windowTitleContainer
        //   |-windowTitle
        //   |-windowClose
        // |-windowContentContainer
        //   |-windowUserContent
        // |-windowResie
        
        // top-level window container, absolutely positioned
        this.windowContainer = document.createElement("div");
        setStyles(this.windowContainer, {
            visibility : "hidden",
            position : "absolute",
            display : "flex",
            flexDirection : "column",
            left : x,
            top : y,
            width : windowWidth,
            height : windowheight,
            background : "white",
            boxShadow  : "0 0 5px 5px rgba(0, 0, 0, 0.4)",
            zIndex : currentMaxZ++
        });
    
        // window title area container, implements window drag
        this.windowTitleContainer = document.createElement("div");
        this.windowContainer.appendChild(this.windowTitleContainer);
        setStyles(this.windowTitleContainer, {
            display : "flex",
            flexDirection : "row",
            height : "35px",
            backgroundColor : "#465dA8",
        });
        installDragHander(this, this.windowTitleContainer, (deltaX, deltaY) => {
            this.windowContainer.style.left = parseInt(this.windowContainer.style.left) + deltaX;
            this.windowContainer.style.top = parseInt(this.windowContainer.style.top) + deltaY;
        })

        // window title text
        this.windowTitle = document.createElement("div");
        this.windowTitleContainer.appendChild(this.windowTitle);
        setStyles(this.windowTitle, {
            display : "flex",
            flex : "1",
            alignItems : "center",
            paddingLeft : "10px",
            paddingRigth : "10px",
            color : "white",
            fontFamily: "Sans-serif",
            userSelect : false,
            cursor : "default",
            unselectable : "on",
            innerText  : "Window",
        });
        
        // window close button
        this.windowClose = document.createElement("button");
        this.windowTitleContainer.appendChild(this.windowClose);
        this.windowClose.innerText = "X";
        setStyles(this.windowClose, {
            display : "flex",
            width : "1px",
            height : "20px",
        });
        this.windowClose.onclick = () => this.close();

        // window content container
        this.windowContentContainer = document.createElement("div");
        this.windowContainer.appendChild(this.windowContentContainer);
        setStyles(this.windowContentContainer, {
            flex : 1,
            minHeight: "60px"
        });

        // window resize area
        this.windowResizeButton = document.createElement("div");
        this.windowContainer.appendChild(this.windowResizeButton);
        setStyles(this.windowResizeButton, {
            position : "absolute",
            backgroundColor : "#FFFFFF50",
            left : parseInt(this.windowContainer.style.width) - resizeHandleSize,
            top : parseInt(this.windowContainer.style.height) - resizeHandleSize,
            width : resizeHandleSize,
            height : resizeHandleSize
        });
        installDragHander(this, this.windowResizeButton, (deltaX, deltaY) => {
            let newWidth = Math.max(windowWidth, parseInt(this.windowContainer.style.width) + deltaX);
            let newHeight = Math.max(windowheight, parseInt(this.windowContainer.style.height) + deltaY);
            this.windowContainer.style.width = newWidth;
            this.windowContainer.style.height = newHeight;
            this.windowResizeButton.style.left = newWidth - resizeHandleSize;
            this.windowResizeButton.style.top = newHeight - resizeHandleSize;
            if (this.resizeCallback !== undefined)
                this.resizeCallback();
        });
    
        windows.push(this.windowContainer);
        root.append(this.windowContainer);
        
        function setStyles(element, styles) {
            for (var style in styles) {
                element.style[style] = styles[style];
            }
        }
        
        function installDragHander(window, element, callback) {
            element.onpointerdown = function(event) {
                this.lastMouseX = event.pageX;
                this.lastMouseY = event.pageY;
                this.setPointerCapture(event.pointerId);
                window.windowContainer.style.zIndex = currentMaxZ++;
            }
            element.onpointermove = function(event) {
                if (this.lastMouseX === undefined)
                    return;
                let deltaX = event.pageX - this.lastMouseX;
                let deltaY = event.pageY - this.lastMouseY;
                this.lastMouseX = event.pageX;
                this.lastMouseY = event.pageY;
                callback(deltaX, deltaY)
            }
            element.onpointerup = function(event) {
                this.lastMouseX = undefined;
                this.lastMouseY = undefined;
                this.releasePointerCapture(event.pointerId);
            }
        }
    }
    
    onClose(callback) {
        this.closeCallback = callback;
    }
    
    close() {
        if (this.closeCallback !== undefined)
            this.closeCallback();
        windows.splice( windows.indexOf(this.windowContainer), 1 );
        root.removeChild(this.windowContainer);
    }
    
    onResize(callback) {
        this.resizeCallback = callback;
    }
    
    get visible() {
        return this.windowContainer.style.visibility == "hidden";
    }

    set visible(visible) {
        this.windowContainer.style.visibility = visible ? "visible" : "hidden"
    }
    
    get title() {
        return this.windowTitle.innerText;
    }

    set title(title) {
        this.windowTitle.innerText = title;
    }
    
    get content() {
        return this.windowContentContainer.firstChild;
    }

    set content(content) {
        if (this.windowContentContainer.firstChild && this.windowContentContainer.firstChild === content)
            return;
        if (this.windowContentContainer.firstChild)
            this.windowContentContainer.removeChild(this.windowContentContainer.firstChild);
        this.windowContentContainer.appendChild(content);
    }
}
