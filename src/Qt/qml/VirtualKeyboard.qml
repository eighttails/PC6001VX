import QtQuick

Item {
    id: root

    property real sceneWidth: keyboard.sceneWidth
    property real sceneHeight: keyboard.sceneHeight
    clip: true

    Rectangle {
        anchors.fill: parent
        color: "black"
    }

    Item {
        id: surface

        width: root.sceneWidth
        height: root.sceneHeight
        scale: root.sceneWidth > 0 && root.sceneHeight > 0
            ? Math.min(root.width / root.sceneWidth, root.height / root.sceneHeight)
            : 1
        x: (root.width - width * scale) / 2
        y: (root.height - height * scale) / 2
        transformOrigin: Item.TopLeft

        Repeater {
            model: keyboard.items

            delegate: Item {
                id: keyRoot

                required property var modelData

                x: modelData.x
                y: modelData.y
                width: modelData.width
                height: modelData.height

                Image {
                    anchors.fill: parent
                    source: modelData.imageSource
                    smooth: true
                }

                Rectangle {
                    anchors.fill: parent
                    color: "blue"
                    opacity: modelData.pressed ? 0.35 : 0
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    preventStealing: true
                    onPressed: mouse => modelData.pointerPressed(mouse.x, mouse.y)
                    onPositionChanged: mouse => {
                        if (pressed) modelData.pointerMoved(mouse.x, mouse.y)
                    }
                    onReleased: mouse => modelData.pointerReleased(mouse.x, mouse.y)
                    onCanceled: modelData.pointerReleased(0, 0)
                }
            }
        }
    }
}
