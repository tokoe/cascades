#include "DropDownModelAdaptor.hpp"

using namespace bb::cascades;

DropDownModelAdaptor::DropDownModelAdaptor(QObject *parent)
    : QObject(parent)
    , m_dropDown(0)
    , m_dataModel(0)
    , m_delegate(0)
{
}

void DropDownModelAdaptor::itemAdded(const QVariantList &indexPath)
{
    if (indexPath.isEmpty())
        return;

    if (!m_dropDown || !m_dataModel || !m_delegate)
        return;

    const int row = indexPath.first().toInt();

    Option* option = optionFromModel(indexPath);
    if (!option)
        return;

    m_dropDown->insert(row, option);
}

void DropDownModelAdaptor::itemUpdated(const QVariantList &indexPath)
{
    if (indexPath.isEmpty())
        return;

    if (!m_dropDown || !m_dataModel)
        return;

    const int row = indexPath.first().toInt();

    updateOption(row);
}

void DropDownModelAdaptor::itemRemoved(const QVariantList &indexPath)
{
    if (indexPath.isEmpty())
        return;

    if (!m_dropDown)
        return;

    const int row = indexPath.first().toInt();

    Option *option = m_dropDown->at(row);
    if (!option)
        return;

    m_dropDown->remove(option);
    option->setParent(0);
    delete option;
}

void DropDownModelAdaptor::itemsChanged(DataModelChangeType::Type changeType, QSharedPointer<DataModel::IndexMapper> indexMapper)
{
    if (changeType == DataModelChangeType::Init) {
        m_dropDown->removeAll();
    } else if (changeType == DataModelChangeType::AddRemove) {
        if (indexMapper.isNull()) {
            m_dropDown->removeAll();
        } else {
            const int childCount = m_dataModel->childCount(QVariantList());
            for (int row = 0; row < childCount; ++row)
                updateOption(row);
        }
    } else if (changeType == DataModelChangeType::Update) {
        const int childCount = m_dataModel->childCount(QVariantList());
        for (int row = 0; row < childCount; ++row)
            updateOption(row);
    }
}

DropDown* DropDownModelAdaptor::dropDown() const
{
    return m_dropDown;
}

void DropDownModelAdaptor::setDropDown(bb::cascades::DropDown* dropDown)
{
    if (m_dropDown == dropDown)
        return;

    m_dropDown = dropDown;
    emit dropDownChanged();

    reset();
}

DataModel* DropDownModelAdaptor::dataModel() const
{
    return m_dataModel;
}

void DropDownModelAdaptor::setDataModel(bb::cascades::DataModel* dataModel)
{
    if (m_dataModel == dataModel)
        return;

    if (m_dataModel) {
        disconnect(m_dataModel, SIGNAL(itemAdded(QVariantList)), this, SLOT(itemAdded(QVariantList)));
        disconnect(m_dataModel, SIGNAL(itemUpdated(QVariantList)), this, SLOT(itemUpdated(QVariantList)));
        disconnect(m_dataModel, SIGNAL(itemRemoved(QVariantList)), this, SLOT(itemRemoved(QVariantList)));
        disconnect(m_dataModel, SIGNAL(itemsChanged(bb::cascades::DataModelChangeType::Type,QSharedPointer<bb::cascades::DataModel::IndexMapper>)),
                   this, SLOT(itemsChanged(bb::cascades::DataModelChangeType::Type,QSharedPointer<bb::cascades::DataModel::IndexMapper>)));
    }

    m_dataModel = dataModel;
    emit dataModelChanged();

    if (m_dataModel) {
        connect(m_dataModel, SIGNAL(itemAdded(QVariantList)), this, SLOT(itemAdded(QVariantList)));
        connect(m_dataModel, SIGNAL(itemUpdated(QVariantList)), this, SLOT(itemUpdated(QVariantList)));
        connect(m_dataModel, SIGNAL(itemRemoved(QVariantList)), this, SLOT(itemRemoved(QVariantList)));
        connect(m_dataModel, SIGNAL(itemsChanged(bb::cascades::DataModelChangeType::Type,QSharedPointer<bb::cascades::DataModel::IndexMapper>)),
                this, SLOT(itemsChanged(bb::cascades::DataModelChangeType::Type,QSharedPointer<bb::cascades::DataModel::IndexMapper>)));
    }

    reset();
}

QDeclarativeComponent* DropDownModelAdaptor::delegate() const
{
    return m_delegate;
}

void DropDownModelAdaptor::setDelegate(QDeclarativeComponent* component)
{
    if (m_delegate == component)
        return;

    m_delegate = component;
    emit delegateChanged();

    reset();
}

void DropDownModelAdaptor::reset()
{
    if (!m_dropDown || !m_dataModel || !m_delegate)
        return;

    m_dropDown->removeAll();

    const int childCount = m_dataModel->childCount(QVariantList());
    for (int row = 0; row < childCount; ++row) {
        Option *option = optionFromModel(QVariantList() << row);
        if (option)
            m_dropDown->add(option);
    }
}

Option* DropDownModelAdaptor::optionFromModel(const QVariantList &indexPath)
{
    const QVariant optionData = m_dataModel->data(indexPath);

    QDeclarativeContext *context = new QDeclarativeContext(m_delegate->creationContext(), this);

    // Make the data from the model available as context property
    context->setContextProperty("OptionIndex", indexPath.first().toInt());
    context->setContextProperty("OptionData", optionData);

    // Create a new Option object from the delegate component
    QObject *object = m_delegate->create(context);
    if (!object) {
         qmlInfo(m_delegate) << tr("Can not create object from delegate");
         delete context;
         return 0;
    }

    Option *option = qobject_cast<Option*>(object);
    if (!option) {
        qmlInfo(m_delegate) << tr("Delegate must be of type Option");
        delete object;
        delete context;
        return 0;
    }

    // keep the pointer to the declarative context
    option->setProperty("__context", QVariant::fromValue(static_cast<QObject*>(context)));

    return option;
}

void DropDownModelAdaptor::updateOption(int row)
{
    Option *option = m_dropDown->at(row);
    if (!option)
        return;

    QDeclarativeContext* context = qobject_cast<QDeclarativeContext*>(option->property("__context").value<QObject*>());
    if (!context)
        return;

    const QVariant optionData = m_dataModel->data(QVariantList() << row);
    context->setContextProperty("OptionIndex", row);
    context->setContextProperty("OptionData", optionData);
}
