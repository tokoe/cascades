#ifndef ABSTRACTITEMMODEL_HPP
#define ABSTRACTITEMMODEL_HPP

#include <bb/cascades/DataModel>

#include <QAbstractItemModel>

class AbstractItemModel : public bb::cascades::DataModel
{
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel *sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)
    Q_PROPERTY(QString itemTypeRole READ itemTypeRole WRITE setItemTypeRole NOTIFY itemTypeRoleChanged)

public:
    AbstractItemModel(QObject *parent = 0);
    ~AbstractItemModel();

    QAbstractItemModel* sourceModel() const;
    void setSourceModel(QAbstractItemModel *sourceModel);

    QString itemTypeRole() const;
    void setItemTypeRole(const QString &roleName);

    int childCount(const QVariantList &indexPath);
    bool hasChildren(const QVariantList &indexPath);
    QString itemType(const QVariantList &indexPath);
    QVariant data(const QVariantList &indexPath);

Q_SIGNALS:
    void sourceModelChanged();
    void itemTypeRoleChanged();

private:
    class Private;
    Private* const d;

    Q_PRIVATE_SLOT(d, void dataChanged(const QModelIndex&, const QModelIndex&))
    Q_PRIVATE_SLOT(d, void headerDataChanged(Qt::Orientation, int, int))
    Q_PRIVATE_SLOT(d, void layoutAboutToBeChanged())
    Q_PRIVATE_SLOT(d, void layoutChanged())
    Q_PRIVATE_SLOT(d, void rowsAboutToBeInserted(const QModelIndex&, int, int))
    Q_PRIVATE_SLOT(d, void rowsInserted(const QModelIndex&, int, int))
    Q_PRIVATE_SLOT(d, void rowsAboutToBeRemoved(const QModelIndex&, int, int))
    Q_PRIVATE_SLOT(d, void rowsRemoved(const QModelIndex&, int, int))
    Q_PRIVATE_SLOT(d, void columnsAboutToBeInserted(const QModelIndex&, int, int))
    Q_PRIVATE_SLOT(d, void columnsInserted(const QModelIndex&, int, int))
    Q_PRIVATE_SLOT(d, void columnsAboutToBeRemoved(const QModelIndex&, int, int))
    Q_PRIVATE_SLOT(d, void columnsRemoved(const QModelIndex&, int, int))
    Q_PRIVATE_SLOT(d, void modelAboutToBeReset())
    Q_PRIVATE_SLOT(d, void modelReset())
    Q_PRIVATE_SLOT(d, void rowsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int))
    Q_PRIVATE_SLOT(d, void rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int))
    Q_PRIVATE_SLOT(d, void columnsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int))
    Q_PRIVATE_SLOT(d, void columnsMoved(const QModelIndex&, int, int, const QModelIndex&, int))
};

#endif
