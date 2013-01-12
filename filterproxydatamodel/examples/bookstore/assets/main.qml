import bb.cascades 1.0

Page {
    Container {
        TextField {
            hintText: qsTr("Filter by author")

            onTextChanging: _filterModel.setAuthorFilter(text)
        }
        ListView {
            dataModel: _filterModel

            listItemComponents: [
                ListItemComponent {
                    type: "item"
                    StandardListItem {
                        title: ListItemData.title
                        description: ListItemData.author
                    }
                }
            ]
        }
    }
}
