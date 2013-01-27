#ifndef FILTERPROXYDATAMODEL_HPP
#define FILTERPROXYDATAMODEL_HPP

#include <bb/cascades/DataModel>

/**
 * @short A filter proxy model for Cascades data model classes.
 *
 * The FilterProxyDataModel can be put between a bb::cascades::DataModel
 * and a ListView to filter out certain items, so that the ListView only
 * shows a subset of the original entries from the source data model.
 *
 * To use the FilterProxyDataModel, inherit from it and reimplement the
 * acceptItem() method.
 */
class FilterProxyDataModel : public bb::cascades::DataModel
{
    Q_OBJECT

public:
    /**
     * Creates a new FilterProxyDataModel object
     *
     * @param parent The parent object.
     */
    FilterProxyDataModel(QObject *parent = 0);

    /**
     * Destroys the FilterProxyDataModel.
     */
    ~FilterProxyDataModel();

    /**
     * Sets the source @p model the filter model should work on.
     */
    void setSourceModel(bb::cascades::DataModel *model);

    /**
     * Returns the source model the filter model works on.
     */
    bb::cascades::DataModel* sourceModel() const;

    // @reimplemented from bb::cascades::DataModel
    int childCount(const QVariantList &indexPath);
    bool hasChildren(const QVariantList &indexPath);
    QString itemType(const QVariantList &indexPath);
    QVariant data(const QVariantList &indexPath);

protected:
    /**
     * This method must be reimplemented to define the filter criterion.
     */
    virtual bool acceptItem(const QVariantList &indexPath, const QString &itemType, const QVariant &data) const;

    /**
     * This method must be called after the filter criterion has changed.
     */
    void invalidateFilter();

private:
    class Private;
    Private* const d;
};

#endif
