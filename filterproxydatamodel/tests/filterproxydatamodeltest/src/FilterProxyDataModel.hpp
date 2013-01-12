#ifndef FILTERPROXYDATAMODEL_HPP
#define FILTERPROXYDATAMODEL_HPP

#include <bb/cascades/DataModel>

class FilterProxyDataModel : public bb::cascades::DataModel
{
    Q_OBJECT

public:
    FilterProxyDataModel(QObject *parent = 0);
    ~FilterProxyDataModel();

    void setSourceModel(bb::cascades::DataModel *sourceModel);
    bb::cascades::DataModel* sourceModel() const;

    // @reimplemented from bb::cascades::DataModel
    int childCount(const QVariantList &indexPath);
    bool hasChildren(const QVariantList &indexPath);
    QString itemType(const QVariantList &indexPath);
    QVariant data(const QVariantList &indexPath);

Q_SIGNALS:
    void hideEmptyGroupsChanged();

protected:
    virtual bool acceptItem(const QVariantList &indexPath, const QString &itemType, const QVariant &data) const;

    void invalidateFilter();

private:
    class Private;
    Private* const d;
};

#endif
