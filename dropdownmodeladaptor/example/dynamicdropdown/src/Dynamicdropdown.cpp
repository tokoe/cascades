#include "Dynamicdropdown.hpp"

#include "DropDownModelAdaptor.hpp"

#include <bb/cascades/AbstractPane>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>

#include <QTimer>

using namespace bb::cascades;

Dynamicdropdown::Dynamicdropdown(bb::cascades::Application *app)
    : QObject(app)
{
    qmlRegisterType<DropDownModelAdaptor>("bb.cascades.ext", 1, 0, "DropDownModelAdaptor");

    m_model = new QVariantListDataModel();
    m_model->setParent(this);

    {
        QVariantMap entry;
        entry["city"] = tr("Dresden");
        entry["offset"] = 1;
        entry["time"] = QString();
        m_model->append(QVariant::fromValue(entry));
    }
    {
        QVariantMap entry;
        entry["city"] = tr("London");
        entry["offset"] = 0;
        entry["time"] = QString();
        m_model->append(QVariant::fromValue(entry));
    }
    {
        QVariantMap entry;
        entry["city"] = tr("Azores");
        entry["offset"] = -1;
        entry["time"] = QString();
        m_model->append(QVariant::fromValue(entry));
    }
    {
        QVariantMap entry;
        entry["city"] = tr("Chihuahua");
        entry["offset"] = -7;
        entry["time"] = QString();
        m_model->append(QVariant::fromValue(entry));
    }

    // create scene document from main.qml asset
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("_model", m_model);

    // create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();
    // set created root object as a scene
    app->setScene(root);

    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), SLOT(timeout()));
    timer->start();
}

void Dynamicdropdown::timeout()
{
    const QDateTime utc = QDateTime::currentDateTime().toUTC();

    for (int row = 0; row < m_model->size(); ++row) {
        QVariantMap entry = m_model->value(row).toMap();
        const int offset = entry.value("offset").toInt();
        const QDateTime localTime = utc.addSecs(60*60*offset);
        entry["time"] = localTime.toString();

        m_model->replace(row, entry);
    }
}
