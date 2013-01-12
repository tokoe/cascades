#include "filterproxydatamodeltest.hpp"

#include "BookFilterModel.hpp"
#include "FilterProxyDataModel.hpp"

#include <bb/cascades/GroupDataModel>

#include <QScopedPointer>
#include <QtTest/QtTest>

using namespace bb::cascades;

static GroupDataModel* createBookModel()
{
    GroupDataModel *bookModel = new GroupDataModel(QStringList() << "title");

    {
        QVariantMap entry;
        entry["author"] = QLatin1String("Jesper Pedersen");
        entry["title"] = QLatin1String("Sams Teach Yourself Emacs in 24 Hours");
        bookModel->insert(entry);
    }
    {
        QVariantMap entry;
        entry["author"] = QLatin1String("Kalle Mathias Dalheimer");
        entry["title"] = QLatin1String("Running Linux");
        bookModel->insert(entry);
    }
    {
        QVariantMap entry;
        entry["author"] = QLatin1String("Jesper Pedersen");
        entry["title"] = QLatin1String("Pratical Qt");
        bookModel->insert(entry);
    }
    {
        QVariantMap entry;
        entry["author"] = QLatin1String("Bjarne Stroustrup");
        entry["title"] = QLatin1String("The C++ Programming Language");
        bookModel->insert(entry);
    }
    {
        QVariantMap entry;
        entry["author"] = QLatin1String("Bjarne Stroustrup");
        entry["title"] = QLatin1String("The Design and Evolution of C++");
        bookModel->insert(entry);
    }
    {
        QVariantMap entry;
        entry["author"] = QLatin1String("Scott Meyers");
        entry["title"] = QLatin1String("Effective C++");
        bookModel->insert(entry);
    }
    {
        QVariantMap entry;
        entry["author"] = QLatin1String("Scott Meyers");
        entry["title"] = QLatin1String("More Effective C++");
        bookModel->insert(entry);
    }

    return bookModel;
}

static QVariantList findModelEntry(const QString &title, const QString &author, GroupDataModel *model)
{
    for (QVariantList indexPath = model->first(); !indexPath.isEmpty(); indexPath = model->after(indexPath)) {
        const QVariantMap item = model->data(indexPath).toMap();
        if ((item["title"].toString() == title) && (item["author"].toString() == author)) {
            return indexPath;
        }
    }

    return QVariantList();
}

void FilterProxyDataModelTest::testEmptySourceModel()
{
    QScopedPointer<GroupDataModel> bookModel(new GroupDataModel(QStringList() << "title"));
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    // verify filter model does not crash if no source model is set
    QVERIFY(filterModel->sourceModel() == 0);
    QCOMPARE(filterModel->childCount(QVariantList()), 0);
    QCOMPARE(filterModel->hasChildren(QVariantList()), false);
    QCOMPARE(filterModel->itemType(QVariantList()), QString());
    QCOMPARE(filterModel->data(QVariantList()), QVariant());

    filterModel->setSourceModel(bookModel.data());

    // verify filter model works correctly with empty source model
    QVERIFY(filterModel->sourceModel() != 0);
    QCOMPARE(filterModel->childCount(QVariantList()), 0);
    QCOMPARE(filterModel->hasChildren(QVariantList()), false);
    QCOMPARE(filterModel->itemType(QVariantList()), bookModel->itemType(QVariantList()));
    QCOMPARE(filterModel->data(QVariantList()), QVariant());
}

void FilterProxyDataModelTest::testSimpleFilter()
{
    QScopedPointer<GroupDataModel> bookModel(createBookModel());
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    filterModel->setSourceModel(bookModel.data());

    // verify that empty filter shows the same items as the source model
    QCOMPARE(filterModel->childCount(QVariantList()), bookModel->childCount(QVariantList()));
    QCOMPARE(filterModel->childCount(QVariantList()), 6);
    QCOMPARE(filterModel->hasChildren(QVariantList()), true);

    // verify that non-empty filter shows only the matching subset
    filterModel->setAuthorFilter(QLatin1String("Scott"));
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));


    // verify that empty filter shows the same items as the source model again
    filterModel->setAuthorFilter(QString());
    QCOMPARE(filterModel->childCount(QVariantList()), bookModel->childCount(QVariantList()));
    QCOMPARE(filterModel->childCount(QVariantList()), 6);
    QCOMPARE(filterModel->hasChildren(QVariantList()), true);

    // verify that child count is correct
    filterModel->setAuthorFilter(QLatin1String("K"));
    QCOMPARE(filterModel->childCount(QVariantList()), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
}

void FilterProxyDataModelTest::testInsertNonMatchToSourceModel1()
{
    // ---- insert non-matching item into new section ----
    QScopedPointer<GroupDataModel> bookModel(createBookModel());
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    filterModel->setSourceModel(bookModel.data());
    filterModel->setAuthorFilter(QLatin1String("Scott"));

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);

    QSignalSpy itemAddedSpy(filterModel.data(), SIGNAL(itemAdded(QVariantList)));

    // insert item that does not match the filter
    {
        QVariantMap entry;
        entry["author"] = QLatin1String("Rainer Grimm");
        entry["title"] = QLatin1String("C++11");
        bookModel->insert(entry);
    }

    // verify that the itemAdded signal is not emitted, since the item is not visible
    QCOMPARE(itemAddedSpy.count(), 0);

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));

    // verify that mapping is still correct after clearing the filter
    filterModel->setAuthorFilter(QString());
    QCOMPARE(filterModel->childCount(QVariantList()), bookModel->childCount(QVariantList()));
    QCOMPARE(filterModel->childCount(QVariantList()), 7);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("C"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->data(QVariantList() << 3).toString(), QString::fromLatin1("P"));
    QCOMPARE(filterModel->data(QVariantList() << 4).toString(), QString::fromLatin1("R"));
    QCOMPARE(filterModel->data(QVariantList() << 5).toString(), QString::fromLatin1("S"));
    QCOMPARE(filterModel->data(QVariantList() << 6).toString(), QString::fromLatin1("T"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 2), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 3), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 4), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 5), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 6), 2);
}

void FilterProxyDataModelTest::testInsertNonMatchToSourceModel2()
{
    // ---- insert non-matching item into visible section ----
    QScopedPointer<GroupDataModel> bookModel(createBookModel());
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    filterModel->setSourceModel(bookModel.data());
    filterModel->setAuthorFilter(QLatin1String("Scott"));

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);

    QSignalSpy itemAddedSpy(filterModel.data(), SIGNAL(itemAdded(QVariantList)));

    // insert item that does not match the filter
    {
        QVariantMap entry;
        entry["author"] = QLatin1String("John Doe");
        entry["title"] = QLatin1String("Most Effective C++");
        bookModel->insert(entry);
    }

    // verify that the itemAdded signal is not emitted, since the item is not visible
    QCOMPARE(itemAddedSpy.count(), 0);

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);

    // verify that mapping is still correct after clearing the filter
    filterModel->setAuthorFilter(QString());
    QCOMPARE(filterModel->childCount(QVariantList()), bookModel->childCount(QVariantList()));
    QCOMPARE(filterModel->childCount(QVariantList()), 6);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("P"));
    QCOMPARE(filterModel->data(QVariantList() << 3).toString(), QString::fromLatin1("R"));
    QCOMPARE(filterModel->data(QVariantList() << 4).toString(), QString::fromLatin1("S"));
    QCOMPARE(filterModel->data(QVariantList() << 5).toString(), QString::fromLatin1("T"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 2);
    QCOMPARE(filterModel->childCount(QVariantList() << 2), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 3), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 4), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 5), 2);
}

void FilterProxyDataModelTest::testInsertMatchToSourceModel1()
{
    // ---- insert matching item into new section ----
    QScopedPointer<GroupDataModel> bookModel(createBookModel());
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    filterModel->setSourceModel(bookModel.data());
    filterModel->setAuthorFilter(QLatin1String("Scott"));

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);

    QSignalSpy itemAddedSpy(filterModel.data(), SIGNAL(itemAdded(QVariantList)));

    // insert item that does match the filter
    {
        QVariantMap entry;
        entry["author"] = QLatin1String("Scottland Yard");
        entry["title"] = QLatin1String("The History of Crime");
        bookModel->insert(entry);
    }

    // verify that the itemAdded signal is emitted once
    QCOMPARE(itemAddedSpy.count(), 2);

    // verify that the two books from Scott Meyers and the new one match
    QCOMPARE(filterModel->childCount(QVariantList()), 3);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("T"));

    // verify that mapping is still correct after clearing the filter
    filterModel->setAuthorFilter(QString());
    QCOMPARE(filterModel->childCount(QVariantList()), bookModel->childCount(QVariantList()));
    QCOMPARE(filterModel->childCount(QVariantList()), 6);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("P"));
    QCOMPARE(filterModel->data(QVariantList() << 3).toString(), QString::fromLatin1("R"));
    QCOMPARE(filterModel->data(QVariantList() << 4).toString(), QString::fromLatin1("S"));
    QCOMPARE(filterModel->data(QVariantList() << 5).toString(), QString::fromLatin1("T"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 2), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 3), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 4), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 5), 3);
}

void FilterProxyDataModelTest::testInsertMatchToSourceModel2()
{
    // ---- insert matching item into visible section ----
    QScopedPointer<GroupDataModel> bookModel(createBookModel());
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    filterModel->setSourceModel(bookModel.data());
    filterModel->setAuthorFilter(QLatin1String("Scott"));

    // verify that the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);

    QSignalSpy itemAddedSpy(filterModel.data(), SIGNAL(itemAdded(QVariantList)));

    // insert item that does match the filter
    {
        QVariantMap entry;
        entry["author"] = QLatin1String("Scottland Yard");
        entry["title"] = QLatin1String("Most of the History of Crime");
        bookModel->insert(entry);
    }

    // verify that the itemAdded signal is emitted once
    QCOMPARE(itemAddedSpy.count(), 1);

    // verify that the two books from Scott Meyers and the new one match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 2);

    // verify that mapping is still correct after clearing the filter
    filterModel->setAuthorFilter(QString());
    QCOMPARE(filterModel->childCount(QVariantList()), bookModel->childCount(QVariantList()));
    QCOMPARE(filterModel->childCount(QVariantList()), 6);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("P"));
    QCOMPARE(filterModel->data(QVariantList() << 3).toString(), QString::fromLatin1("R"));
    QCOMPARE(filterModel->data(QVariantList() << 4).toString(), QString::fromLatin1("S"));
    QCOMPARE(filterModel->data(QVariantList() << 5).toString(), QString::fromLatin1("T"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 2);
    QCOMPARE(filterModel->childCount(QVariantList() << 2), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 3), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 4), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 5), 2);
}

void FilterProxyDataModelTest::testRemoveNonMatchFromSourceModel1()
{
    // ---- remove non-matching item from section with only this item ----
    QScopedPointer<GroupDataModel> bookModel(createBookModel());
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    filterModel->setSourceModel(bookModel.data());
    filterModel->setAuthorFilter(QLatin1String("Scott"));

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);

    QSignalSpy itemRemovedSpy(filterModel.data(), SIGNAL(itemRemoved(QVariantList)));

    // remove item that does not match the filter
    const QVariantList indexPath = findModelEntry("Running Linux", "Kalle Mathias Dalheimer", bookModel.data());
    bookModel->removeAt(indexPath);

    // verify that the itemRemoved signal is not emitted
    QCOMPARE(itemRemovedSpy.count(), 0);

    // verify that the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));

    // verify that mapping is still correct after clearing the filter
    filterModel->setAuthorFilter(QString());
    QCOMPARE(filterModel->childCount(QVariantList()), 5);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("P"));
    QCOMPARE(filterModel->data(QVariantList() << 3).toString(), QString::fromLatin1("S"));
    QCOMPARE(filterModel->data(QVariantList() << 4).toString(), QString::fromLatin1("T"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 2), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 3), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 4), 2);
}

void FilterProxyDataModelTest::testRemoveNonMatchFromSourceModel2()
{
    // ---- remove non-matching item from section with multiple items ----
    QScopedPointer<GroupDataModel> bookModel(createBookModel());
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    {
        QVariantMap entry;
        entry["author"] = QLatin1String("John Doe");
        entry["title"] = QLatin1String("My Castle");
        bookModel->insert(entry);
    }

    filterModel->setSourceModel(bookModel.data());

    // verify it the new entry shows up correctly
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 2);

    filterModel->setAuthorFilter(QLatin1String("Scott"));

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);

    QSignalSpy itemRemovedSpy(filterModel.data(), SIGNAL(itemRemoved(QVariantList)));

    // remove item that does not match the filter
    const QVariantList indexPath = findModelEntry("My Castle", "John Doe", bookModel.data());
    bookModel->removeAt(indexPath);

    // verify that the itemRemoved signal is not emitted
    QCOMPARE(itemRemovedSpy.count(), 0);

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);

    // verify that mapping is still correct after clearing the filter
    filterModel->setAuthorFilter(QString());
    QCOMPARE(filterModel->childCount(QVariantList()), 6);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("P"));
    QCOMPARE(filterModel->data(QVariantList() << 3).toString(), QString::fromLatin1("R"));
    QCOMPARE(filterModel->data(QVariantList() << 4).toString(), QString::fromLatin1("S"));
    QCOMPARE(filterModel->data(QVariantList() << 5).toString(), QString::fromLatin1("T"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 2), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 3), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 4), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 5), 2);
}

void FilterProxyDataModelTest::testRemoveMatchFromSourceModel1()
{
    // ---- remove matching item from section with only this item ----
    QScopedPointer<GroupDataModel> bookModel(createBookModel());
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    filterModel->setSourceModel(bookModel.data());
    filterModel->setAuthorFilter(QLatin1String("Scott"));

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);

    QSignalSpy itemRemovedSpy(filterModel.data(), SIGNAL(itemRemoved(QVariantList)));

    // remove item that does match the filter
    const QVariantList indexPath = findModelEntry("More Effective C++", "Scott Meyers", bookModel.data());
    bookModel->removeAt(indexPath);

    // verify that the itemRemoved signal is not emitted
    QCOMPARE(itemRemovedSpy.count(), 2);

    // verify that the remaining book from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 1);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));

    // verify that mapping is still correct after clearing the filter
    filterModel->setAuthorFilter(QString());
    QCOMPARE(filterModel->childCount(QVariantList()), 5);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("P"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("R"));
    QCOMPARE(filterModel->data(QVariantList() << 3).toString(), QString::fromLatin1("S"));
    QCOMPARE(filterModel->data(QVariantList() << 4).toString(), QString::fromLatin1("T"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 2), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 3), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 4), 2);
}

void FilterProxyDataModelTest::testRemoveMatchFromSourceModel2()
{
    // ---- remove matching item from section with this item and a visible one ----
    QScopedPointer<GroupDataModel> bookModel(createBookModel());
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    {
        QVariantMap entry;
        entry["author"] = QLatin1String("Scott Meyers");
        entry["title"] = QLatin1String("Effective STL");
        bookModel->insert(entry);
    }

    filterModel->setSourceModel(bookModel.data());
    filterModel->setAuthorFilter(QLatin1String("Scott"));

    // verify that only the three books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 2);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);

    QSignalSpy itemRemovedSpy(filterModel.data(), SIGNAL(itemRemoved(QVariantList)));

    // remove item that does match the filter
    const QVariantList indexPath = findModelEntry("Effective STL", "Scott Meyers", bookModel.data());
    bookModel->removeAt(indexPath);

    // verify that the itemRemoved signal is not emitted
    QCOMPARE(itemRemovedSpy.count(), 1);

    // verify that the remaining books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);

    // verify that mapping is still correct after clearing the filter
    filterModel->setAuthorFilter(QString());
    QCOMPARE(filterModel->childCount(QVariantList()), 6);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("P"));
    QCOMPARE(filterModel->data(QVariantList() << 3).toString(), QString::fromLatin1("R"));
    QCOMPARE(filterModel->data(QVariantList() << 4).toString(), QString::fromLatin1("S"));
    QCOMPARE(filterModel->data(QVariantList() << 5).toString(), QString::fromLatin1("T"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 2), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 3), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 4), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 5), 2);
}

void FilterProxyDataModelTest::testRemoveMatchFromSourceModel3()
{
    // ---- remove matching item from section with this item and a hidden one ----
    QScopedPointer<GroupDataModel> bookModel(createBookModel());
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    {
        QVariantMap entry;
        entry["author"] = QLatin1String("Scott Meyers");
        entry["title"] = QLatin1String("PDF version of Effective STL");
        bookModel->insert(entry);
    }

    filterModel->setSourceModel(bookModel.data());
    filterModel->setAuthorFilter(QLatin1String("Scott"));

    // verify that only the three books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 3);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("P"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 2), 1);

    QSignalSpy itemRemovedSpy(filterModel.data(), SIGNAL(itemRemoved(QVariantList)));

    // remove item that does match the filter
    const QVariantList indexPath = findModelEntry("PDF version of Effective STL", "Scott Meyers", bookModel.data());
    bookModel->removeAt(indexPath);

    // verify that the itemRemoved signal is not emitted
    QCOMPARE(itemRemovedSpy.count(), 2);

    // verify that the remaining books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);

    // verify that mapping is still correct after clearing the filter
    filterModel->setAuthorFilter(QString());
    QCOMPARE(filterModel->childCount(QVariantList()), 6);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("P"));
    QCOMPARE(filterModel->data(QVariantList() << 3).toString(), QString::fromLatin1("R"));
    QCOMPARE(filterModel->data(QVariantList() << 4).toString(), QString::fromLatin1("S"));
    QCOMPARE(filterModel->data(QVariantList() << 5).toString(), QString::fromLatin1("T"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 2), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 3), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 4), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 5), 2);
}

void FilterProxyDataModelTest::testUpdateNonMatchInSourceModel1()
{
    // ---- update hidden item in hidden section to hidden item ----
    QScopedPointer<GroupDataModel> bookModel(createBookModel());
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    filterModel->setSourceModel(bookModel.data());
    filterModel->setAuthorFilter(QLatin1String("Scott"));

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);

    QSignalSpy itemAddedSpy(filterModel.data(), SIGNAL(itemAdded(QVariantList)));
    QSignalSpy itemUpdatedSpy(filterModel.data(), SIGNAL(itemUpdated(QVariantList)));
    QSignalSpy itemRemovedSpy(filterModel.data(), SIGNAL(itemRemoved(QVariantList)));

    // update item that does not match the filter
    {
        const QVariantList indexPath = findModelEntry("Running Linux", "Kalle Mathias Dalheimer", bookModel.data());
        QVariantMap entry = bookModel->data(indexPath).toMap();

        entry["title"] = "Running Linux, again";
        bookModel->updateItem(indexPath, entry);
    }

    // verify that no signal is emitted, since the item is not visible
    QCOMPARE(itemAddedSpy.count(), 0);
    QCOMPARE(itemUpdatedSpy.count(), 0);
    QCOMPARE(itemRemovedSpy.count(), 0);

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);

    // verify that mapping is still correct after clearing the filter
    filterModel->setAuthorFilter(QString());
    QCOMPARE(filterModel->childCount(QVariantList()), bookModel->childCount(QVariantList()));
    QCOMPARE(filterModel->childCount(QVariantList()), 6);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("P"));
    QCOMPARE(filterModel->data(QVariantList() << 3).toString(), QString::fromLatin1("R"));
    QCOMPARE(filterModel->data(QVariantList() << 4).toString(), QString::fromLatin1("S"));
    QCOMPARE(filterModel->data(QVariantList() << 5).toString(), QString::fromLatin1("T"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 2), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 3), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 4), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 5), 2);
}

void FilterProxyDataModelTest::testUpdateNonMatchInSourceModel2()
{
    // ---- update hidden item in visible section to hidden item ----
    QScopedPointer<GroupDataModel> bookModel(createBookModel());
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    {
        QVariantMap entry;
        entry["author"] = QLatin1String("Andrei Alexandrescu");
        entry["title"] = QLatin1String("Modern C++ Design");
        bookModel->insert(entry);
    }

    filterModel->setSourceModel(bookModel.data());
    filterModel->setAuthorFilter(QLatin1String("Scott"));

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);

    QSignalSpy itemAddedSpy(filterModel.data(), SIGNAL(itemAdded(QVariantList)));
    QSignalSpy itemUpdatedSpy(filterModel.data(), SIGNAL(itemUpdated(QVariantList)));
    QSignalSpy itemRemovedSpy(filterModel.data(), SIGNAL(itemRemoved(QVariantList)));

    // update item that does not match the filter
    {
        const QVariantList indexPath = findModelEntry("Modern C++ Design", "Andrei Alexandrescu", bookModel.data());
        QVariantMap entry = bookModel->data(indexPath).toMap();
        entry["title"] = "Even moderner C++ Design";
        bookModel->updateItem(indexPath, entry);
    }

    // verify that no signal is emitted, since the item is not visible
    QCOMPARE(itemAddedSpy.count(), 0);
    QCOMPARE(itemUpdatedSpy.count(), 0);
    QCOMPARE(itemRemovedSpy.count(), 0);

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);

    // verify that mapping is still correct after clearing the filter
    filterModel->setAuthorFilter(QString());
    QCOMPARE(filterModel->childCount(QVariantList()), bookModel->childCount(QVariantList()));
    QCOMPARE(filterModel->childCount(QVariantList()), 6);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("P"));
    QCOMPARE(filterModel->data(QVariantList() << 3).toString(), QString::fromLatin1("R"));
    QCOMPARE(filterModel->data(QVariantList() << 4).toString(), QString::fromLatin1("S"));
    QCOMPARE(filterModel->data(QVariantList() << 5).toString(), QString::fromLatin1("T"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 2);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 2), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 3), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 4), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 5), 2);
}

void FilterProxyDataModelTest::testUpdateMatchInSourceModel1()
{
    // ---- update hidden item in visible section to visible item ----
    QScopedPointer<GroupDataModel> bookModel(createBookModel());
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    {
        QVariantMap entry;
        entry["author"] = QLatin1String("Andrei Alexandrescu");
        entry["title"] = QLatin1String("Modern C++ Design");
        bookModel->insert(entry);
    }

    filterModel->setSourceModel(bookModel.data());
    filterModel->setAuthorFilter(QLatin1String("Scott"));

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);

    QSignalSpy itemAddedSpy(filterModel.data(), SIGNAL(itemAdded(QVariantList)));
    QSignalSpy itemUpdatedSpy(filterModel.data(), SIGNAL(itemUpdated(QVariantList)));
    QSignalSpy itemRemovedSpy(filterModel.data(), SIGNAL(itemRemoved(QVariantList)));

    // update item that does not match the filter
    {
        const QVariantList indexPath = findModelEntry("Modern C++ Design", "Andrei Alexandrescu", bookModel.data());
        QVariantMap entry = bookModel->data(indexPath).toMap();
        entry["author"] = "Scott Meyers";
        bookModel->updateItem(indexPath, entry);
    }

    // verify that no signal is emitted, since the item is not visible
    QCOMPARE(itemAddedSpy.count(), 1);
    QCOMPARE(itemUpdatedSpy.count(), 0);
    QCOMPARE(itemRemovedSpy.count(), 0);

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 2);

    // verify that mapping is still correct after clearing the filter
    filterModel->setAuthorFilter(QString());
    QCOMPARE(filterModel->childCount(QVariantList()), bookModel->childCount(QVariantList()));
    QCOMPARE(filterModel->childCount(QVariantList()), 6);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("P"));
    QCOMPARE(filterModel->data(QVariantList() << 3).toString(), QString::fromLatin1("R"));
    QCOMPARE(filterModel->data(QVariantList() << 4).toString(), QString::fromLatin1("S"));
    QCOMPARE(filterModel->data(QVariantList() << 5).toString(), QString::fromLatin1("T"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 2);
    QCOMPARE(filterModel->childCount(QVariantList() << 2), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 3), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 4), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 5), 2);
}

void FilterProxyDataModelTest::testUpdateMatchInSourceModel2()
{
    // ---- update hidden item in visible section to visible item ----
    QScopedPointer<GroupDataModel> bookModel(createBookModel());
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    filterModel->setSourceModel(bookModel.data());
    filterModel->setAuthorFilter(QLatin1String("Scott"));

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);

    QSignalSpy itemAddedSpy(filterModel.data(), SIGNAL(itemAdded(QVariantList)));
    QSignalSpy itemUpdatedSpy(filterModel.data(), SIGNAL(itemUpdated(QVariantList)));
    QSignalSpy itemRemovedSpy(filterModel.data(), SIGNAL(itemRemoved(QVariantList)));

    // update item that does not match the filter
    {
        const QVariantList indexPath = findModelEntry("More Effective C++", "Scott Meyers", bookModel.data());
        QVariantMap entry = bookModel->data(indexPath).toMap();
        entry["author"] = "Andrei Alexandrescu";
        bookModel->updateItem(indexPath, entry);
    }

    // verify that no signal is emitted, since the item is not visible
    QCOMPARE(itemAddedSpy.count(), 0);
    QCOMPARE(itemUpdatedSpy.count(), 0);
    QCOMPARE(itemRemovedSpy.count(), 2);

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 1);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);

    // verify that mapping is still correct after clearing the filter
    filterModel->setAuthorFilter(QString());
    QCOMPARE(filterModel->childCount(QVariantList()), bookModel->childCount(QVariantList()));
    QCOMPARE(filterModel->childCount(QVariantList()), 6);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("P"));
    QCOMPARE(filterModel->data(QVariantList() << 3).toString(), QString::fromLatin1("R"));
    QCOMPARE(filterModel->data(QVariantList() << 4).toString(), QString::fromLatin1("S"));
    QCOMPARE(filterModel->data(QVariantList() << 5).toString(), QString::fromLatin1("T"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 2), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 3), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 4), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 5), 2);
}

void FilterProxyDataModelTest::testUpdateMatchInSourceModel3()
{
    // ---- update hidden item in visible section to visible item ----
    QScopedPointer<GroupDataModel> bookModel(createBookModel());
    QScopedPointer<BookFilterModel> filterModel(new BookFilterModel());

    filterModel->setSourceModel(bookModel.data());
    filterModel->setAuthorFilter(QLatin1String("Scott"));

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 2);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("M"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);

    QSignalSpy itemAddedSpy(filterModel.data(), SIGNAL(itemAdded(QVariantList)));
    QSignalSpy itemUpdatedSpy(filterModel.data(), SIGNAL(itemUpdated(QVariantList)));
    QSignalSpy itemRemovedSpy(filterModel.data(), SIGNAL(itemRemoved(QVariantList)));

    // update item that does not match the filter
    {
        const QVariantList indexPath = findModelEntry("More Effective C++", "Scott Meyers", bookModel.data());
        QVariantMap entry = bookModel->data(indexPath).toMap();
        entry["title"] = "Extended More Effective C++";
        bookModel->updateItem(indexPath, entry);
    }

    // verify that no signal is emitted, since the item is not visible
    QCOMPARE(itemAddedSpy.count(), 1);
    QCOMPARE(itemUpdatedSpy.count(), 0);
    QCOMPARE(itemRemovedSpy.count(), 2);

    // verify that only the two books from Scott Meyers match
    QCOMPARE(filterModel->childCount(QVariantList()), 1);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 2);

    // verify that mapping is still correct after clearing the filter
    filterModel->setAuthorFilter(QString());
    QCOMPARE(filterModel->childCount(QVariantList()), bookModel->childCount(QVariantList()));
    QCOMPARE(filterModel->childCount(QVariantList()), 5);
    QCOMPARE(filterModel->data(QVariantList() << 0).toString(), QString::fromLatin1("E"));
    QCOMPARE(filterModel->data(QVariantList() << 1).toString(), QString::fromLatin1("P"));
    QCOMPARE(filterModel->data(QVariantList() << 2).toString(), QString::fromLatin1("R"));
    QCOMPARE(filterModel->data(QVariantList() << 3).toString(), QString::fromLatin1("S"));
    QCOMPARE(filterModel->data(QVariantList() << 4).toString(), QString::fromLatin1("T"));
    QCOMPARE(filterModel->childCount(QVariantList() << 0), 2);
    QCOMPARE(filterModel->childCount(QVariantList() << 1), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 2), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 3), 1);
    QCOMPARE(filterModel->childCount(QVariantList() << 4), 2);
}
