#include "FilterProxyDataModel.hpp"

#include "FilterProxyDataModel_p.hpp"

using namespace bb::cascades;

FilterProxyDataModel::Private::Private(FilterProxyDataModel *qq)
    : q(qq)
    , m_sourceModel(0)
    , m_rootMapping(new Mapping(false, 0))
{
}

FilterProxyDataModel::Private::~Private()
{
    delete m_rootMapping;
}

void FilterProxyDataModel::Private::createMapping(Mapping *parentMapping, const QVariantList &parentIndexPath)
{
    for (int row = 0; row < m_sourceModel->childCount(parentIndexPath); ++row) {
        const QVariantList indexPath = (QVariantList(parentIndexPath) << row);
        const bool accepted = q->acceptItem(indexPath, m_sourceModel->itemType(indexPath), m_sourceModel->data(indexPath));

        Mapping *mapping = new Mapping(accepted, parentMapping);
        parentMapping->children.append(mapping);
        createMapping(mapping, indexPath);
    }
}

void FilterProxyDataModel::Private::refreshAll()
{
    qDeleteAll(m_rootMapping->children);
    m_rootMapping->children.clear();

    if (!m_sourceModel)
        return;

    createMapping(m_rootMapping, QVariantList());
}

QVariantList FilterProxyDataModel::Private::mapToSource(const QVariantList &indexPath) const
{
    QVariantList result;

    Mapping *mapping = m_rootMapping;
    for (int level = 0; level < indexPath.count(); ++level) {
        Mapping *childMapping = mapping->visibleChild(indexPath[level].toInt());
        if (!childMapping)
            return QVariantList();

        result << QVariant(childMapping->atRow());

        mapping = childMapping;
    }

    return result;
}

QVariantList FilterProxyDataModel::Private::mapFromSource(const QVariantList &indexPath) const
{
    QVariantList result;

    Mapping *mapping = m_rootMapping;
    for (int level = 0; level < indexPath.count(); ++level) {
        Mapping *childMapping = mapping->children[indexPath[level].toInt()];

        result << childMapping->atVisibleRow();

        mapping = childMapping;
    }

    return result;
}

Mapping* FilterProxyDataModel::Private::mappingForSourceIndex(const QVariantList &indexPath) const
{
    Mapping *mapping = m_rootMapping;

    for (int level = 0; level < indexPath.count(); ++level)
        mapping = mapping->children[indexPath[level].toInt()];

    return mapping;
}

void FilterProxyDataModel::Private::createMappingForSourceIndex(const QVariantList &indexPath, bool visible)
{
    Mapping *mapping = m_rootMapping;

    // iterate to the parent mapping where the new mapping should be insert
    for (int level = 0; level < indexPath.count() - 1; ++level)
        mapping = mapping->children[indexPath[level].toInt()];

    // create the new mapping and insert it
    Mapping *newMapping = new Mapping(visible, mapping);
    mapping->children.insert(indexPath[indexPath.count() - 1].toInt(), newMapping);
}

void FilterProxyDataModel::Private::updateMappingForSourceIndex(const QVariantList &indexPath, bool visible)
{
    Mapping *mapping = mappingForSourceIndex(indexPath);

    mapping->visible = visible;
}

void FilterProxyDataModel::Private::removeMappingForSourceIndex(const QVariantList &indexPath)
{
    Mapping *mapping = mappingForSourceIndex(indexPath);

    mapping->parent->children.removeAll(mapping);
    delete mapping;
}

void FilterProxyDataModel::Private::dumpMapping(Mapping *mapping, int level)
{
    const QString prefix = QString().fill(QLatin1Char(' '), level);
    for (int row = 0; row < mapping->children.count(); ++row) {
        const QString data = m_sourceModel->data(mapping->children.at(row)->sourceIndex()).toString();
        qDebug("%s%s: %s", qPrintable(prefix), qPrintable(data), mapping->children.at(row)->visible ? "true" : "false");
        dumpMapping(mapping->children.at(row), level + 2);
    }
}

void FilterProxyDataModel::Private::updateParentChainAfterAdd(const QVariantList &sourceIndexPath)
{
    Mapping *mapping = m_rootMapping;
    QVariantList partIndexPath;
    for (int level = 0; level < sourceIndexPath.count(); ++level) {
        partIndexPath << sourceIndexPath[level];
        mapping = mapping->children[sourceIndexPath[level].toInt()];

        const bool accepted = q->acceptItem(partIndexPath, m_sourceModel->itemType(partIndexPath), m_sourceModel->data(partIndexPath));

        if (accepted && !mapping->visible) {
            mapping->visible = true;
            emit q->itemAdded(mapFromSource(partIndexPath));
        }
    }
}

void FilterProxyDataModel::Private::updateParentChainAfterRemove(Mapping *parentMapping)
{
    Mapping *mapping = parentMapping;

    while (mapping->parent) {
        const QVariantList indexPath = mapping->sourceIndex();
        const bool accepted = q->acceptItem(indexPath, m_sourceModel->itemType(indexPath), m_sourceModel->data(indexPath));

        if (mapping->visible && !accepted) {
            mapping->visible = false;
            emit q->itemRemoved(mapFromSource(indexPath));
        }

        mapping = mapping->parent;
    }
}

void FilterProxyDataModel::Private::sourceModelItemAdded(const QVariantList &indexPath)
{
    const bool accepted = q->acceptItem(indexPath, m_sourceModel->itemType(indexPath), m_sourceModel->data(indexPath));
    createMappingForSourceIndex(indexPath, false);

    if (accepted)
        updateParentChainAfterAdd(indexPath);
}

void FilterProxyDataModel::Private::sourceModelItemUpdated(const QVariantList &indexPath)
{
    const bool accepted = q->acceptItem(indexPath, m_sourceModel->itemType(indexPath), m_sourceModel->data(indexPath));
    const Mapping *mapping = mappingForSourceIndex(indexPath);
    if (mapping->visible) { // item was visible
        if (accepted) { // item is now visible
            emit q->itemUpdated(mapFromSource(indexPath));
        } else { // item is now hidden
            updateMappingForSourceIndex(indexPath, false);
            emit q->itemRemoved(mapFromSource(indexPath));
            updateParentChainAfterRemove(mapping->parent);
        }
    } else { // item was hidden
        if (accepted) { // item is now visible
            updateMappingForSourceIndex(indexPath, false);
            updateParentChainAfterAdd(indexPath);
        } else {
            // nothing to do
        }
    }
}

void FilterProxyDataModel::Private::sourceModelItemRemoved(const QVariantList &indexPath)
{
    Mapping *mapping = mappingForSourceIndex(indexPath);
    Mapping *parentMapping = mapping->parent;
    const bool isVisible = mapping->visible;
    const QVariantList localIndexPath = mapFromSource(indexPath);

    removeMappingForSourceIndex(indexPath);

    if (isVisible) {
        emit q->itemRemoved(localIndexPath);
        updateParentChainAfterRemove(parentMapping);
    }
}

void FilterProxyDataModel::Private::sourceModelItemsChanged(bb::cascades::DataModelChangeType::Type eChangeType,
                                                            const QSharedPointer<bb::cascades::DataModel::IndexMapper> &indexMapper)
{
    Q_UNUSED(eChangeType)
    Q_UNUSED(indexMapper)
    qWarning() << "sourceModelItemsChanged called";
}


FilterProxyDataModel::FilterProxyDataModel(QObject *parent)
    : DataModel(parent)
    , d(new Private(this))
{
}

FilterProxyDataModel::~FilterProxyDataModel()
{
    delete d;
}

void FilterProxyDataModel::setSourceModel(bb::cascades::DataModel *sourceModel)
{
    if (d->m_sourceModel) {
        disconnect(d->m_sourceModel, SIGNAL(itemAdded(QVariantList)),
                   d, SLOT(sourceModelItemAdded(QVariantList)));
        disconnect(d->m_sourceModel, SIGNAL(itemUpdated(QVariantList)),
                   d, SLOT(sourceModelItemUpdated(QVariantList)));
        disconnect(d->m_sourceModel, SIGNAL(itemRemoved(QVariantList)),
                   d, SLOT(sourceModelItemRemoved(QVariantList)));
        disconnect(d->m_sourceModel, SIGNAL(itemsChanged(bb::cascades::DataModelChangeType::Type,QSharedPointer<bb::cascades::DataModel::IndexMapper>)),
                   d, SLOT(sourceModelItemsChanged(bb::cascades::DataModelChangeType::Type,QSharedPointer<bb::cascades::DataModel::IndexMapper>)));
    }

    d->m_sourceModel = sourceModel;

    if (d->m_sourceModel) {
        connect(d->m_sourceModel, SIGNAL(itemAdded(QVariantList)),
                d, SLOT(sourceModelItemAdded(QVariantList)));
        connect(d->m_sourceModel, SIGNAL(itemUpdated(QVariantList)),
                d, SLOT(sourceModelItemUpdated(QVariantList)));
        connect(d->m_sourceModel, SIGNAL(itemRemoved(QVariantList)),
                d, SLOT(sourceModelItemRemoved(QVariantList)));
        connect(d->m_sourceModel, SIGNAL(itemsChanged(bb::cascades::DataModelChangeType::Type,QSharedPointer<bb::cascades::DataModel::IndexMapper>)),
                d, SLOT(sourceModelItemsChanged(bb::cascades::DataModelChangeType::Type,QSharedPointer<bb::cascades::DataModel::IndexMapper>)));
    }

    invalidateFilter();
}

bb::cascades::DataModel* FilterProxyDataModel::sourceModel() const
{
    return d->m_sourceModel;
}

bool FilterProxyDataModel::acceptItem(const QVariantList &indexPath, const QString &itemType, const QVariant &data) const
{
    Q_UNUSED(indexPath)
    Q_UNUSED(itemType)
    Q_UNUSED(data)

    return true;
}

void FilterProxyDataModel::invalidateFilter()
{
    d->refreshAll();
    emit itemsChanged(bb::cascades::DataModelChangeType::Init, QSharedPointer<bb::cascades::DataModel::IndexMapper>(0));
}

int FilterProxyDataModel::childCount(const QVariantList &indexPath)
{
    if (!d->m_sourceModel)
        return 0;

    return d->mappingForSourceIndex(d->mapToSource(indexPath))->visibleChildCount();
}

bool FilterProxyDataModel::hasChildren(const QVariantList &indexPath)
{
    if (!d->m_sourceModel)
        return false;

    return (childCount(indexPath) != 0);
}

QString FilterProxyDataModel::itemType(const QVariantList &indexPath)
{
    if (!d->m_sourceModel)
        return QString();

    return d->m_sourceModel->itemType(d->mapToSource(indexPath));
}

QVariant FilterProxyDataModel::data(const QVariantList &indexPath)
{
    if (!d->m_sourceModel)
        return QVariant();

    return d->m_sourceModel->data(d->mapToSource(indexPath));
}
