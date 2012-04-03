import QtQuick 1.0
import "WaitDialog" as Comp

Rectangle {
    id: container

    width: 360
    height: 640
    color: "transparent"

    // Wait indicator is visible by default
    Comp.WaitIndicator {
        id: waitIndicator

        // Always define the size for the WaitIndicator.
        anchors.fill: parent

        // Start showing the WaitIndicator immediately.
        // Otherwise it'll wait for one second before starting the indicator.
        delay: 0

        // Make sure it's on top of everything, so that the
        // Fader will capture key clicks
        z: 120

        // In real application, the WaitIndicator could be bound to
        // some "loading" appState variable etc.
        show: true
    }
}
