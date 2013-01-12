#ifndef BOOKFILTERMODEL_HPP
#define BOOKFILTERMODEL_HPP

#include "FilterProxyDataModel.hpp"

class BookFilterModel : public FilterProxyDataModel
{
    Q_OBJECT

public:
    BookFilterModel(QObject *parent = 0);

public Q_SLOTS:
    void setAuthorFilter(const QString &author);

protected:
    bool acceptItem(const QVariantList &indexPath, const QString &itemType, const QVariant &data) const;

private:
    QString m_authorFilter;
};

#endif
