import bb.cascades 1.0
import com.kdab.components 1.0

Page {
    attachedObjects: [
        AbstractItemModel {
            id: abstractItemModel

            sourceModel: _fileSystemModel
        }
    ]

    Container {
        Button {
            text: qsTr("Back")
            enabled: (listView.rootIndexPath.length != 0)

            onClicked: {
                var indexPath = listView.rootIndexPath
                indexPath.pop()
                listView.rootIndexPath = indexPath
            }
        }

        ListView {
            id: listView

            dataModel: abstractItemModel

            layout: StackListLayout {
                headerMode: ListHeaderMode.None
            }

            listItemComponents: [
                ListItemComponent {
                    type: ""

                    StandardListItem {
                        title: ListItemData.display
                    }
                }
            ]

            onTriggered: {
                listView.rootIndexPath = indexPath
                abstractItemModel.fetchMore(indexPath)
            }
        }
    }
}

