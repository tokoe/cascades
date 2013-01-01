#ifndef DROPDOWNMODELADAPTOR_HPP
#define DROPDOWNMODELADAPTOR_HPP

#include <bb/cascades/DataModel>
#include <bb/cascades/DropDown>

#include <QDeclarativeComponent>
#include <QObject>

/**
 * @short An adaptor class to let the DropDown use a DataModel as source
 */
class DropDownModelAdaptor : public QObject
{
    Q_OBJECT

    // The DropDown to put the options in
    Q_PROPERTY(bb::cascades::DropDown* dropDown READ dropDown WRITE setDropDown NOTIFY dropDownChanged)

    // The DataModel that is used as data source
    Q_PROPERTY(bb::cascades::DataModel* dataModel READ dataModel WRITE setDataModel NOTIFY dataModelChanged)

    // The Options delegate that is used as a template for all model entries
    Q_PROPERTY(QDeclarativeComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)

    // Mark the 'delegate' property as default property
    Q_CLASSINFO("DefaultProperty", "delegate")

public:
    // Creates a new DropDownModelAdaptor
    DropDownModelAdaptor(QObject *parent = 0);

Q_SIGNALS:
    // The change notification signals of the properties
    void dropDownChanged();
    void dataModelChanged();
    void delegateChanged();

private Q_SLOTS:
    void itemAdded(const QVariantList &indexPath);
    void itemUpdated(const QVariantList &indexPath);
    void itemRemoved(const QVariantList &indexPath);
    void itemsChanged(bb::cascades::DataModelChangeType::Type, QSharedPointer<bb::cascades::DataModel::IndexMapper>);

private:
    // The accessor methods of the properties
    bb::cascades::DropDown* dropDown() const;
    void setDropDown(bb::cascades::DropDown* dropDown);
    bb::cascades::DataModel* dataModel() const;
    void setDataModel(bb::cascades::DataModel* dataModel);
    QDeclarativeComponent* delegate() const;
    void setDelegate(QDeclarativeComponent* component);

    void reset();
    bb::cascades::Option* optionFromModel(const QVariantList &indexPath);
    void updateOption(int row);

    // The property values
    QPointer<bb::cascades::DropDown> m_dropDown;
    QPointer<bb::cascades::DataModel> m_dataModel;
    QPointer<QDeclarativeComponent> m_delegate;
};

#endif
