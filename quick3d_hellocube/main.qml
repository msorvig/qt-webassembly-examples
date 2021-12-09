/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick
import QtQuick.Window
import QtQuick3D

Window {
    id: window
    width: 640
    height: 640
    visible: true
    color: "black"

    Rectangle {
        id: qt_logo
        width: 230
        height: 230
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
        color: "transparent"

        //! [offscreenSurface]
        layer.enabled: true
        //! [offscreenSurface]

        Rectangle {
            anchors.fill: parent
            color: "black"

            //! [2d]
            Image {
                anchors.fill: parent
                source: "qt_logo.png"
            }
            Text {
                id: text
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                color: "white"
                font.pixelSize: 17
                text: qsTr("The Future is Written with Qt")
            }
            //! [2d]

            //! [2danimation]
            transform: Rotation {
                id: rotation
                origin.x: qt_logo.width / 2
                origin.y: qt_logo.height / 2
                axis { x: 1; y: 0; z: 0 }
            }

            PropertyAnimation {
                id: flip1
                target: rotation
                property: "angle"
                duration: 600
                to: 180
                from: 0
            }
            PropertyAnimation {
                id: flip2
                target: rotation
                property: "angle"
                duration: 600
                to: 360
                from: 180
            }
            //! [2danimation]
        }
    }

    View3D {
        id: view
        anchors.fill: parent
        camera: camera
        renderMode: View3D.Overlay

        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(0, 200, 300)
            eulerRotation.x: -30
        }

        DirectionalLight {
            eulerRotation.x: -30
        }

        Model {
            //! [3dcube]
            id: cube
            visible: true
            position: Qt.vector3d(0, 0, 0)
            source: "#Cube"
            materials: [ DefaultMaterial {
                    diffuseMap: Texture {
                        id: texture
                        sourceItem: qt_logo
                        flipV: true
                    }
                }
            ]
            eulerRotation.y: 90
            //! [3dcube]

            SequentialAnimation on eulerRotation {
                loops: Animation.Infinite
                PropertyAnimation {
                    duration: 5000
                    from: Qt.vector3d(0, 0, 0)
                    to: Qt.vector3d(360, 0, 360)
                }
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: qt_logo

        Text {
            id: clickme
            anchors.top: mouseArea.top
            anchors.horizontalCenter: mouseArea.horizontalCenter
            font.pixelSize: 17
            text: "Click me!"
            color: "white"

            SequentialAnimation on color {
                loops: Animation.Infinite
                ColorAnimation { duration: 400; from: "white"; to: "black" }
                ColorAnimation { duration: 400; from: "black"; to: "white" }
            }

            states: [
                State {
                    name: "flipped";
                    AnchorChanges {
                        target: clickme
                        anchors.top: undefined
                        anchors.bottom: mouseArea.bottom
                    }
                }
            ]
        }

        onClicked: {
            if (clickme.state == "flipped") {
                clickme.state = "";
                flip2.start();
            } else {
                clickme.state = "flipped";
                flip1.start();
            }
        }
    }
}