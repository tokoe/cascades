#include "BookFilterModel.hpp"

BookFilterModel::BookFilterModel(QObject *parent)
    : FilterProxyDataModel(parent)
{
}

void BookFilterModel::setAuthorFilter(const QString &author)
{
    m_authorFilter = author;

    invalidateFilter();
}

bool BookFilterModel::acceptItem(const QVariantList &indexPath, const QString &itemType, const QVariant &data) const
{
    if (itemType == QLatin1String("header")) {
        // Hide header items if none of their child items match
        bool hasAcceptableItem = false;
        for (int row = 0; row < sourceModel()->childCount(indexPath); ++row) {
            const QVariantList childIndexPath = (QVariantList(indexPath) << row);
            if (acceptItem(childIndexPath, sourceModel()->itemType(childIndexPath), sourceModel()->data(childIndexPath))) {
                hasAcceptableItem = true;
                break;
            }
        }

        return hasAcceptableItem;
    } else {
        return (m_authorFilter.isEmpty() ||
                data.toMap()["author"].toString().startsWith(m_authorFilter, Qt::CaseInsensitive));
    }
}
