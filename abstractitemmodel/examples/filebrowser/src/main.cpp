#include "AbstractItemModel.hpp"

#include <bb/cascades/AbstractPane>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>

#include <QtGui/QFileSystemModel>
#include <QLocale>
#include <QTranslator>

using namespace bb::cascades;

Q_DECL_EXPORT int main(int argc, char **argv)
{
    qmlRegisterType<QAbstractItemModel>();
    qmlRegisterType<AbstractItemModel>("com.kdab.components", 1, 0, "AbstractItemModel");

    Application app(argc, argv);

    QTranslator translator;
    const QString filename = QString::fromLatin1("filebrowser_%1").arg(QLocale().name());
    if (translator.load(filename, "app/native/qm"))
        app.installTranslator( &translator );

    QFileSystemModel *model = new QFileSystemModel(&app);
    model->setRootPath("app/");

    QmlDocument *qml = QmlDocument::create("asset:///main.qml");
    qml->setContextProperty("_fileSystemModel", model);

    AbstractPane *root = qml->createRootObject<AbstractPane>();
    app.setScene(root);

    return app.exec();
}
