#include "cascadestester.hpp"

#include <bb/cascades/Application>

#include <QtTest/QTest>

CascadesTester::CascadesTester(QObject *object, int argc, char **argv)
    : m_object(object), m_argc(argc), m_argv(argv)
{

}

void CascadesTester::startTest()
{
    // start the unit test
    const int result = QTest::qExec(m_object, 1, m_argv);

    // terminate application
    bb::cascades::Application::exit(result);
}
