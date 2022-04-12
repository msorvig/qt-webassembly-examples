import QtQuick

Rectangle {
     width: 250
     height: 250
     color: "steelblue"
     property alias fpsText: fps.text

     Rectangle {
         y: 100
         x: 100
         width: 200
         height: 200
         RotationAnimation on rotation {
             loops: Animation.Infinite
             duration: 2000
             from: 0
             to: 360
        }
         
         gradient: Gradient {
             GradientStop { position: 0.0; color: "lightseagreen" }
             GradientStop { position: 1.0; color: "cornflowerblue" }
         }
    }

    Text {
        id: "fps"
        y: 20
        x: 20
        text: "FPS: -"
    }
}
