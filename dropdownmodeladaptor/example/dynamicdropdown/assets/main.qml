import bb.cascades 1.0

// allow access to DropDownModelAdaptor
import bb.cascades.ext 1.0

Page {
    Container {
        DropDown {
            id: myDropDown

            attachedObjects: [
                DropDownModelAdaptor {
                    dropDown: myDropDown
                    dataModel: _model

                    Option {
                        text: OptionData.city
                        description: OptionData.time
                        value: OptionData.time
                    }
                }
            ]
        }

        Label {
            text: myDropDown.selectedValue
        }
    }
}

