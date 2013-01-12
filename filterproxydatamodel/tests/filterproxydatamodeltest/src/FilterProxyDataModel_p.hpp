#ifndef FILTERPROXYDATAMODEL_P_H
#define FILTERPROXYDATAMODEL_P_H

#include "FilterProxyDataModel.hpp"

struct Mapping
{
    Mapping(bool _visible, Mapping *_parent)
        : visible(_visible)
        , parent(_parent)
    {
    }

    ~Mapping()
    {
        qDeleteAll(children);
        children.clear();
    }

    typedef QList<Mapping*> List;

    int visibleChildCount() const
    {
        int count = 0;
        for (int i = 0; i < children.count(); ++i) {
            if (children[i]->visible)
                count++;
        }

        return count;
    }

    QVariantList sourceIndex() const
    {
        QVariantList result;
        const Mapping *mapping = this;
        while (mapping->parent != 0) {
            result.prepend(mapping->atRow());
            mapping = mapping->parent;
        }

        return result;
    }

    Mapping* visibleChild(int row) const
    {
        int visibleRow = 0;
        for (int i = 0; i < children.count(); ++i) {
            if (children[i]->visible) {
                if (visibleRow == row)
                    return children[i];

                visibleRow++;
            }
        }

        Q_ASSERT(false); // should never happen
        return 0;
    }

    int atVisibleRow() const
    {
        int row = 0;
        for (int i = 0; i < parent->children.count(); ++i) {
            if (parent->children[i] == this)
                return row;

            if (parent->children[i]->visible)
                row++;
        }

        Q_ASSERT(false); // should never happen
        return 0;
    }

    int atRow() const
    {
        return parent->children.indexOf(const_cast<Mapping*>(this));
    }

    bool visible;
    Mapping* parent;
    List children;
};

class FilterProxyDataModel::Private : public QObject
{
    Q_OBJECT

public:
    Private(FilterProxyDataModel *qq);
    ~Private();

    FilterProxyDataModel* q;
    bb::cascades::DataModel* m_sourceModel;

    Mapping* m_rootMapping;
    void createMapping(Mapping *parentMapping, const QVariantList &parentIndexPath);
    void refreshAll();

    QVariantList mapToSource(const QVariantList &indexPath) const;
    QVariantList mapFromSource(const QVariantList &indexPath) const;

    Mapping* mappingForSourceIndex(const QVariantList &indexPath) const;
    void createMappingForSourceIndex(const QVariantList &indexPath, bool visible);
    void updateMappingForSourceIndex(const QVariantList &indexPath, bool visible);
    void removeMappingForSourceIndex(const QVariantList &indexPath);

    void dumpMapping(Mapping*, int);
    void updateParentChainAfterAdd(const QVariantList &sourceIndexPath);
    void updateParentChainAfterRemove(Mapping *parentMapping);

public Q_SLOTS:
    void sourceModelItemAdded(const QVariantList &indexPath);
    void sourceModelItemUpdated(const QVariantList &indexPath);
    void sourceModelItemRemoved(const QVariantList &indexPath);
    void sourceModelItemsChanged(bb::cascades::DataModelChangeType::Type eChangeType, const QSharedPointer<bb::cascades::DataModel::IndexMapper> &indexMapper);
};

#endif
