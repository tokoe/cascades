#ifndef Dynamicdropdown_HPP_
#define Dynamicdropdown_HPP_

#include <bb/cascades/Application>
#include <bb/cascades/QListDataModel>

#include <QObject>

class Dynamicdropdown : public QObject
{
    Q_OBJECT
public:
    Dynamicdropdown(bb::cascades::Application *app);
    virtual ~Dynamicdropdown() {}

private Q_SLOTS:
    void timeout();

private:
    bb::cascades::QVariantListDataModel* m_model;
};


#endif
