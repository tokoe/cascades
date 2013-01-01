#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include <QLocale>
#include <QTranslator>
#include <Qt/qdeclarativedebug.h>

#include "Dynamicdropdown.hpp"

using namespace bb::cascades;

Q_DECL_EXPORT int main(int argc, char **argv)
{
    Application app(argc, argv);

    // localization support
    QTranslator translator;
    const QString locale_string = QLocale().name();
    const QString filename = QString::fromLatin1("dynamicdropdown_%1").arg(locale_string);
    if (translator.load(filename, "app/native/qm")) {
        app.installTranslator( &translator );
    }

    new Dynamicdropdown(&app);

    return Application::exec();
}
