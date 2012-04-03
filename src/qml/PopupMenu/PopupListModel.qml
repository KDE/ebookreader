import QtQuick 1.0

ListModel {
    id: model

    ListElement {
        itemTitle: "NOT SET"
    }
    ListElement {
        itemTitle: "NOT SET"
    }
    ListElement {
        itemTitle: "NOT SET"
    }
    ListElement {
        itemTitle: "NOT SET"
    }
    ListElement {
        itemTitle: "NOT SET"
    }
    ListElement {
        itemTitle: "NOT SET"
    }

    function title(index) {
        if (title["text"] === undefined) {
            title.text = [
                qsTr("Open"),
                qsTr("Normal Screen"),
                qsTr("Go To Page"),
                qsTr("Zoom"),
                qsTr("Properties"),
                qsTr("Exit")
            ]
        }
        return title.text[index]
    }

}

