import QtQuick 2.0
import Sailfish.Silica 1.0
import QtSparql 1.0
import harbour.orn 1.0

Page {
    // Duplicate count property because items are not really deleted from the model
    property int _count: 0

    id: page
    allowedOrientations: defaultAllowedOrientations

    SparqlListModel {
        id: queryModel
        query: "SELECT nfo:fileName(?r) as ?fileName strafter(nie:url(?r), 'file://') as ?filePath WHERE { ?r nie:mimeType 'application/x-rpm' }"
        connection: SparqlConnection {
            driver: "QTRACKER_DIRECT"
        }

        onStatusChanged: {
            if (status === SparqlListModel.Ready) {
                listView.model = queryModel
                busyIndicator.running = false
                _count = count
            }
        }
    }

    SilicaListView {
        id: listView
        anchors.fill: parent

        model: busyIndicator.running ? null : queryModel

        header: PageHeader {
            //% "Local RPM files"
            title: qsTrId("orn-local-rpms")
        }

        delegate: ListItem {
            id: delegateItem
            width: parent.width
            contentHeight: delegateColumn.height + Theme.paddingSmall * 2
            onClicked: openMenu()

            menu: ContextMenu {

                MenuItem {
                    text: qsTrId("orn-install")
                    onClicked: delegateItem.remorseAction(qsTrId("orn-installing"), function() {
                        OrnPm.installFile(filePath)
                    })
                }

                MenuItem {
                    //% "Delete"
                    text: qsTrId("orn-delete")
                    //% "Deleting"
                    onClicked: delegateItem.remorseAction(qsTrId("orn-deleting"), function() {
                        if (Storeman.removeFile(filePath)) {
                            delegateItem.animateRemoval()
                            _count -= 1
                        } else {
                            //% "Failed to delete"
                            notification.showPopup(qsTrId("orn-deletion-error"),
                                                   filePath, "image://theme/icon-lock-warning")
                        }
                    })
                }
            }

            Column {
                id: delegateColumn
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    right: parent.right
                    margins: Theme.horizontalPageMargin
                }

                Label {
                    width: parent.width
                    maximumLineCount: 1
                    truncationMode: TruncationMode.Fade
                    color: delegateItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                    text: fileName
                }

                Label {
                    width: parent.width
                    maximumLineCount: 1
                    truncationMode: TruncationMode.Fade
                    color: delegateItem.highlighted ? Theme.highlightColor : Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                    text: filePath
                }
            }
        }

        PullDownMenu {
            MenuItem {
                text: qsTrId("orn-refresh")
                onClicked: {
                    listView.model = null
                    busyIndicator.running = true
                    queryModel.reload()
                }
            }
        }

        VerticalScrollDecorator { }

        BusyIndicator {
            id: busyIndicator
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: true
        }

        ViewPlaceholder {
            enabled: !busyIndicator.running && _count === 0
            //% "No local RPM files were found"
            text: qsTrId("orn-no-local-rpms")
            hintText: qsTrId("orn-pull-refresh")
        }
    }
}
