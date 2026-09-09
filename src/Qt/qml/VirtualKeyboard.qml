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

                // MouseAreaはタッチを合成マウスイベントに変換するため同時に1点しか
                // 扱えず、複数キーの同時押し(SHIFT+一般キー等)ができない。
                // TapHandlerはキー(Item)ごとに独立してタッチポイントを追跡できるため
                // 複数キーの同時押しに対応できる。
                // マウス/タッチの区別は押下時にのみ判定してlastIsTouchへ保存し、
                // 解放時はその値を再利用する(解放時点ではpoint.deviceの型情報が
                // 参照できないことがあるため)。
                property bool lastIsTouch: false

                TapHandler {
                    id: tapHandler
                    acceptedButtons: Qt.LeftButton
                    acceptedDevices: PointerDevice.AllDevices
                    gesturePolicy: TapHandler.WithinBounds
                    onPressedChanged: {
                        if (pressed) {
                            keyRoot.lastIsTouch = point.device
                                && point.device.type !== PointerDevice.Mouse
                            modelData.pointerPressed(point.position.x, point.position.y, keyRoot.lastIsTouch)
                        } else {
                            modelData.pointerReleased(point.position.x, point.position.y, keyRoot.lastIsTouch)
                        }
                    }
                    onCanceled: modelData.pointerReleased(0, 0, keyRoot.lastIsTouch)
                }
            }
        }
    }
}
