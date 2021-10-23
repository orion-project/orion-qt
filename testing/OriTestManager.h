#ifndef ORI_TEST_MANAGER_H
#define ORI_TEST_MANAGER_H

#include "OriTestWindow.h"
#include "OriTestBase.h"

#include <QApplication>
#include <QDateTime>

namespace Ori {
namespace Testing {

inline bool isTesting()
{
    return QApplication::arguments().contains("test");
}

inline bool noGui()
{
    return QApplication::arguments().contains("nogui");
}

inline int runConsole(TestSuite tests)
{
    TestLogger::enable(true);
    TestSession session(tests);
    session.run();
    return 0;
}

inline int runWindow(QApplication& app, TestSuite tests)
{
    TestWindow w;
    w.setTests(tests);
    w.show();
    return app.exec();
}

inline int run(QApplication& app, TestSuite tests)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    qsrand(QDateTime::currentDateTime().toTime_t());
#endif
    int result = noGui()
        ? runConsole(tests)
        : runWindow(app, tests);

    qDeleteAll(tests);
    return result;
}

inline int run(QApplication& app, std::initializer_list<TestSuite> suites)
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
