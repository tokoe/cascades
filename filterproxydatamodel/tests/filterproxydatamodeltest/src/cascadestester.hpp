#ifndef CASCADESTESTER_HPP
#define CASCADESTESTER_HPP

#include <bb/cascades/Application>
#include <bb/cascades/Page>

#include <QtCore/QObject>
#include <QtTest/QtTest>

/**
 * @short A helper class to execute QTestLib based unit tests in Cascades
 *
 * This class executes a QTestLib based unit test inside the main event loop
 * of Cascades, so that Cascades controls can be tested.
 */
class CascadesTester : public QObject
{
    Q_OBJECT

    public:
        /**
         * Creates a new cascades tester object.
         *
         * @param object The unit test object.
         * @param argc The count of the command line arguments.
         * @param argv The command line arguments.
         */
        explicit CascadesTester(QObject *object, int argc = 0, char **argv = 0);

    public Q_SLOTS:
        /**
         * Starts the execution of the unit test.
         */
        void startTest();

    private:
        QObject *m_object;
        int m_argc;
        char **m_argv;
};

#define QTEST_CASCADES_MAIN(TestObject) \
int main(int argc, char **argv) \
{ \
    bb::cascades::Application app(argc, argv); \
    TestObject tc; \
    CascadesTester tester(&tc, argc, argv); \
    bb::cascades::Application::instance()->setScene(new bb::cascades::Page()); \
    QMetaObject::invokeMethod(&tester, "startTest", Qt::QueuedConnection); \
    return bb::cascades::Application::exec(); \
}

#endif
