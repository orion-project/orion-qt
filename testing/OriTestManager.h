#ifndef ORI_TEST_MANAGER_H
#define ORI_TEST_MANAGER_H

#include "OriTestWindow.h"
#include "OriTestBase.h"

#include <QApplication>
#include <QDateTime>

namespace Ori {
namespace Testing {

bool isTesting()
{
    return QApplication::arguments().contains("test");
}

bool noGui()
{
    return QApplication::arguments().contains("nogui");
}

int runConsole(TestSuite tests)
{
    TestLogger::enable(true);
    TestSession session;
    session.run(tests);
    return 0;
}

int runWindow(QApplication& app, TestSuite tests)
{
    TestWindow w;
    w.setTests(tests);
    w.show();
    return app.exec();
}

int run(QApplication& app, TestSuite tests)
{
    qsrand(QDateTime::currentDateTime().toTime_t());

    int result = noGui()
        ? runConsole(tests)
        : runWindow(app, tests);

    free(tests);
    return result;
}

int run(QApplication& app, std::initializer_list<TestSuite> suites)
{
    TestSuite tests;

    for (auto suite: suites)
        for (auto test : suite)
            tests.append(test);

    return run(app, tests);
}

} // namespace Testing
} // namespace Ori

#endif // ORI_TEST_MANAGER_H
