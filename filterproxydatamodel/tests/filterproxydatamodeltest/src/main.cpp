#include "cascadestester.hpp"
#include "filterproxydatamodeltest.hpp"

#include <iostream>

static void customMessageHandler(QtMsgType type, const char *message)
{
    switch (type) {
        case QtDebugMsg:
            std::cout << "Debug: " << message << std::endl;
            break;
        case QtWarningMsg:
            std::cout << "Warning: " << message << std::endl;
            break;
        case QtCriticalMsg:
            std::cout << "Critical: " << message << std::endl;
            break;
        case QtFatalMsg:
            std::cout << "Fatal: " << message << std::endl;
            std::abort();
            break;
    }
}

static bool executeOnStartup()
{
    qInstallMsgHandler(customMessageHandler);
    return true;
}

static bool s_dummy = executeOnStartup();

QTEST_CASCADES_MAIN(FilterProxyDataModelTest)
