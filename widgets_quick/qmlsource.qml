import QtQuick 2.0

Rectangle {
    color: "steelblue"

    Text {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 15
        text: "Hello from QQuick(Widget)!"
        color: "white"
        
        RotationAnimation on rotation {
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 1000
        }    
    }
}

