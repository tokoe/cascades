#include <bb/cascades/AbstractPane>
#include <bb/cascades/Application>
#include <bb/cascades/GroupDataModel>
#include <bb/cascades/QmlDocument>

#include <QLocale>
#include <QTranslator>

#include "BookFilterModel.hpp"

using namespace bb::cascades;

Q_DECL_EXPORT int main(int argc, char **argv)
{
    Application app(argc, argv);

    QTranslator translator;
    const QString filename = QString::fromLatin1("bookstore_%1").arg(QLocale().name());
    if (translator.load(filename, "app/native/qm"))
        app.installTranslator( &translator );

    GroupDataModel *bookModel = new GroupDataModel(QStringList() << "title", &app);

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

    BookFilterModel *filterModel = new BookFilterModel(&app);
    filterModel->setSourceModel(bookModel);

    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(&app);
    qml->setContextProperty("_filterModel", filterModel);

    AbstractPane *root = qml->createRootObject<AbstractPane>();
    app.setScene(root);

    return app.exec();
}
