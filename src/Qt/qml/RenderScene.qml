import QtQuick
import PC6001VX 1.0

Item {
    id: root

    property real sceneWidth: 0
    property real sceneHeight: 0
    property bool fixMagnification: false
    property real magnification: 1.0
    property real tiltAngle: 0

    Rectangle {
        anchors.fill: parent
        color: "black"
    }

    Item {
        id: surface

        width: root.sceneWidth
        height: root.sceneHeight
        scale: root.sceneWidth > 0 && root.sceneHeight > 0
            ? (root.fixMagnification
                ? root.magnification
                : Math.min(root.width / root.sceneWidth, root.height / root.sceneHeight))
            : 1
        x: (root.width - width * scale) / 2
        y: (root.height - height * scale) / 2
        transformOrigin: Item.TopLeft

        RenderCanvas {
            id: canvas
            objectName: "renderCanvas"

            x: paintLeft
            y: paintTop
            width: paintWidth
            height: paintHeight
            sceneWidth: root.sceneWidth
            sceneHeight: root.sceneHeight
            transform: Rotation {
                origin.x: root.sceneWidth / 2 - canvas.paintLeft
                origin.y: root.sceneHeight / 2 - canvas.paintTop
                angle: root.tiltAngle
            }
        }
    }
}
