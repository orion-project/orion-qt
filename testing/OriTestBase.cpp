#include "OriTestBase.h"

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QStringList>
#include <QTextStream>

#include <chrono>

namespace Ori {
namespace Testing {

#ifdef Q_OS_WIN
// In MinGW (7.3 at the moment) on Windows, the system timer is used
// for `std::chrono::high_resolution_clock` which has a bad resolution.
// This implementation of the true high resolution clock for Windows is from here:
// https://stackoverflow.com/questions/16299029/resolution-of-stdchronohigh-resolution-clock-doesnt-correspond-to-measureme
#include <windows.h>
namespace
{
    const int64_t __frequency = []() -> int64_t
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        return frequency.QuadPart;
    }();

    struct WindowsHighResClock
    {
        typedef std::chrono::duration<int64_t, std::nano> duration;
        typedef std::chrono::time_point<WindowsHighResClock> time_point;
        static time_point now()
        {
            LARGE_INTEGER count;
            QueryPerformanceCounter(&count);
            return time_point(duration(count.QuadPart * static_cast<int64_t>(std::nano::den) / __frequency));
        }
    };
} // namespace
using HighResClock = WindowsHighResClock;
#else
using HighResClock = std::chrono::high_resolution_clock;
#endif

TestGroup* asGroup(TestBase* test)
{
    return dynamic_cast<TestGroup*>(test);
}

QString formatDuration(int64_t duration_ns)
{
    double d = duration_ns;
    if (d/1e3 < 1)
        return QString("%1 ns").arg(duration_ns);
    if (d/1e6 < 1)
        return QString("%1 µs").arg(d/1e3, 0, 'f', 3);
    if (d/1e9 < 1)
        return QString("%1 ms").arg(d/1e6, 0, 'f', 3);
    return QString("%1 s").arg(d/1e9, 0, 'f', 3);
}

//------------------------------------------------------------------------------
//                                 TestLogger
//------------------------------------------------------------------------------

bool TestLogger::_enabled = false;

void TestLogger::reset()
{
    if (QFile::exists(fileName()))
        QFile::remove(fileName());
}

void TestLogger::write(const QString& msg)
{
    // TODO: Such open-write-close logging slows down the test execution
    // (e.g. a session finishes in 4s without logs and in 24s with logs).
    // Should be replaced with writing into memory buffer
    // but concern is we won't see any logs if test session crashes
    // so it might be better to move file writing in a separate thread
    QFile file(fileName());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return;

    QTextStream out(&file);
    out << msg << "\n";

    file.close();
}

#define TEST_LOGGER(str) if (TestLogger::enabled()) TestLogger::write(str)

//------------------------------------------------------------------------------
//                                 TestSession
//------------------------------------------------------------------------------

// *************** SESSION ***************
// STAT: tests: 99, passed: 99, failed: 99
// ***************** END ******************
// **************** START *****************
// **************** TEST ******************
// *************** PASSED *****************
// *************** FAILED *****************

TestSession::TestSession(const TestSuite& tests): _tests(tests)
{
    if (TestLogger::enabled())
        TestLogger::reset();

    TEST_LOGGER(QString("*************** SESSION ***************\n"
                        "Started at %1\n")
                      .arg(QDateTime::currentDateTime().toString()));
}

TestSession::~TestSession()
{
    TEST_LOGGER(QString("STAT: tests: %1, passed: %2, failed: %3\n"
                        "Summary tests duration: %4\n"
                        "Total session duration: %5\n"
                        "***************** END ******************\n")
                      .arg(_testsRun).arg(_testsPass).arg(_testsFail)
                      .arg(formatDuration(_testsDuration))
                      .arg(formatDuration(_sessionDuration)));
}

void TestSession::run()
{
    _testsRun = 0;
    _testsFail = 0;
    _testsPass = 0;
    _sessionDuration = 0;
    _testsDuration = 0;
    _stopRequested = false;

    auto start = HighResClock::now();

    for (auto test : _tests)
    {
        test->reset();

        auto parentGroup = asGroup(test->parent());
        if (parentGroup)
        {
            if (parentGroup->_beforeAll)
                parentGroup->_beforeAll->reset();
            if (parentGroup->_afterAll)
                parentGroup->_afterAll->reset();
        }
    }

    runGroup(_tests);

    auto stop = HighResClock::now();
    _sessionDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();

    if (emitSignals) emit sessionFinished();
}

void TestSession::runGroup(const TestSuite& tests)
{
    int count = tests.size();
    for (int i = 0; i < count; i++)
    {
        runTest(tests.at(i), i == count-1);

        if (_stopRequested)
            return;
    }
}

void TestSession::runTest(TestBase *test, bool isLastInGroup)
{
    TestGroup *parentGroup = asGroup(test->parent());

    if (parentGroup and parentGroup->_beforeAll)
    {
        if (parentGroup->_beforeAll->result() == TestResult::None)
        {
            parentGroup->_beforeAll->runTest();
            if (!parentGroup->log().isEmpty())
                parentGroup->logMessage("---------------");
        }
        if (parentGroup->_beforeAll->result() != TestResult::Pass)
            return;
    }

    TestGroup *group = asGroup(test);
    if (group)
    {
        notifyTestRunning(group);

        runGroup(group->tests());

        notifyTestFinished(group);
    }
    else
    {
        notifyTestRunning(test);

        _testsRun++;

        bool runTest = true;
        if (parentGroup and parentGroup->_beforeEach)
        {
            parentGroup->_beforeEach->reset();
            parentGroup->_beforeEach->run();
            if (parentGroup->_beforeEach->result() == TestResult::Fail)
            {
                runTest = false;
                test->setResult(false);
                test->setMessage(QStringLiteral("BEFORE_EACH: %1").arg(parentGroup->_beforeEach->message()));
            }
            if (!parentGroup->_beforeEach->log().isEmpty())
            {
                test->logMessage(QStringLiteral("---------------"));
                test->logMessage(QStringLiteral("BEFORE_EACH:"));
                test->logMessage(parentGroup->_beforeEach->log());
                test->logMessage(QStringLiteral("---------------"));
            }
        }

        if (runTest)
        {
            test->runTest();

            if (parentGroup and parentGroup->_afterEach)
            {
                parentGroup->_afterEach->reset();
                parentGroup->_afterEach->run();
                if (parentGroup->_afterEach->result() == TestResult::Fail)
                {
                    test->setResult(false);
                    if (test->message().isEmpty())
                        test->setMessage(QStringLiteral("AFTER_EACH: %1").arg(parentGroup->_afterEach->message()));
                    else
                        test->logMessage(QStringLiteral("AFTER_EACH: %1").arg(parentGroup->_afterEach->message()));
                }
                if (!parentGroup->_afterEach->log().isEmpty())
                {
                    test->logMessage(QStringLiteral("---------------"));
                    test->logMessage(QStringLiteral("AFTER_EACH:"));
                    test->logMessage(parentGroup->_afterEach->log());
                    test->logMessage(QStringLiteral("---------------"));
                }
            }
        }

        switch (test->result())
        {
        case TestResult::Pass: _testsPass++; break;
        case TestResult::Fail: _testsFail++; break;
        default: break;
        }

        _testsDuration += test->duration();
        TEST_LOGGER(QStringLiteral("Executed in %1").arg(formatDuration(test->duration())));

        notifyTestFinished(test);
    }

    if (parentGroup and parentGroup->_afterAll and isLastInGroup)
        parentGroup->_afterAll->runTest();
}

void TestSession::notifyTestRunning(TestBase* test)
{
    if (TestLogger::enabled())
    {
        if (test->kind() == TestKind::Group)
            TestLogger::write(QStringLiteral("################ GROUP ################\n%1\n").arg(test->path()));
        else
            TestLogger::write(QStringLiteral("**************** TEST ******************\n%1\n").arg(test->path()));
    }

    if (emitSignals)
        emit testRunning(test);
}

void TestSession::notifyTestFinished(TestBase* test)
{
    if (emitSignals)
        emit testFinished(test);

    if (test->kind() == TestKind::Test)
    {
        switch (test->result())
        {
        case TestResult::Pass:
            TEST_LOGGER(QStringLiteral("*************** PASSED *****************\n"));
            break;
        case TestResult::Fail:
            TEST_LOGGER(QStringLiteral("*************** FAILED *****************\n"));
            break;
        case TestResult::None:
            TEST_LOGGER(QStringLiteral("*************** UNKNOWN *****************\n"));
            break;
        }
    }
}

int TestSession::countTestsInGroup(const TestSuite& tests) const
{
    int count = 0;
    for (auto test : tests)
    {
        switch (test->kind())
        {
        case TestKind::Test:
            count++;
            break;
        case TestKind::Group:
            count += countTestsInGroup(asGroup(test)->tests());
            break;
        default:
            break;
        }
    }
    return count;
}

//------------------------------------------------------------------------------
//                                 TestGroup
//------------------------------------------------------------------------------

TestGroup::TestGroup(const char *name, std::initializer_list<TestBase*> args)
    : TestBase(name, nullptr, TestKind::Group)
{
    for (auto arg : args) append(arg);
}

void TestGroup::append(TestBase *test)
{
    switch (test->kind())
    {
    case TestKind::BeforeAll: _beforeAll = test; break;
    case TestKind::AfterAll: _afterAll = test; break;
    case TestKind::BeforeEach: _beforeEach = test; break;
    case TestKind::AfterEach: _afterEach = test; break;
    default: _tests.push_back(test); break;
    }
    test->_parent = this;
}

void TestGroup::reset()
{
    TestBase::reset();

    for (auto test : _tests)
        test->reset();

    if (_beforeAll) _beforeAll->reset();
    if (_afterAll) _afterAll->reset();
    if (_beforeEach) _beforeEach->reset();
    if (_afterEach) _afterEach->reset();
}

int64_t TestGroup::duration() const
{
    int64_t total = 0;
    for (auto test : _tests)
        total += test->duration();
    return total;
}

//------------------------------------------------------------------------------
//                                 TestBase
//------------------------------------------------------------------------------

TestBase::TestBase(const char *name, TestMethod method, TestKind kind)
    : _name(name), _method(method), _kind(kind) {}

TestBase::~TestBase() {}

QString TestBase::path() const
{
    QString path = name();

    if (_parent)
        path = _parent->path() + " \\ " + path;

    return path;
}

void TestBase::setResult(bool pass)
{
    auto res = pass ? TestResult::Pass : TestResult::Fail;
    if (_result == res) return;
    _result = res;

    if (_parent)
    {
        if (_parent->result() == TestResult::None)
            _parent->setResult(pass);
        else if (!pass)
            _parent->setResult(false);

        if (!pass)
        {
            switch (_kind)
            {
            case TestKind::Test:
            case TestKind::Group:
                if (_parent->message().isEmpty())
                    _parent->setMessage(QStringLiteral("Some of children tests failed"));
                break;
            default:
                break;
            }
        }
    }
}

void TestBase::setMessage(const QString& msg)
{
    _message = msg;

    if (_parent)
    {
        if (_kind == TestKind::BeforeAll)
            _parent->setMessage(QStringLiteral("BEFORE_ALL: %1").arg(msg));
        else if (_kind == TestKind::AfterAll)
        {
            if (_parent->message().isEmpty())
                _parent->setMessage(QStringLiteral("AFTER_ALL: %1").arg(msg));
            else
                _parent->logMessage(QStringLiteral("AFTER_ALL: %1").arg(msg));
        }
    }
}

void TestBase::reset()
{
    _duration_ns = 0;
    _result = TestResult::None;
    _data.clear();
    _message.clear();
    _log.clear();
}

void TestBase::runTest()
{
    auto start = HighResClock::now();

    run();

    auto stop = HighResClock::now();
    _duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();

    // Assertions only assign Fail result
    if (_result != TestResult::Fail)
        setResult(true);
}

void TestBase::run()
{
    if (_method)
        _method(this);
}

void TestBase::logAssertion(const QString& assertion, const QString& condition,
                            const QString& expected, const QString& actual,
                            const QString &file, int line)
{
    QString msg;
    QTextStream stream(&msg);
    stream << "Assertion : " << assertion << '\n'
           << "Condition : " << condition << '\n'
           << "Expected  : " << expected << '\n'
           << "Actual    : " << actual << '\n'
           << "Location  : " << file << ':' << line << '\n';

    if (_parent)
    {
        if (_kind == TestKind::BeforeAll)
            _parent->logMessage(QString("BEFORE_ALL: Assertion:\n%1").arg(msg));
        else if (_kind == TestKind::AfterAll)
            _parent->logMessage(QString("AFTER_ALL: Assertion:\n%1").arg(msg));
        else _log.append(msg);
    }
    else _log.append(msg);
}

void TestBase::logMessage(const QString& msg)
{
    TEST_LOGGER(msg);

    if (_parent)
    {
        if (_kind == TestKind::BeforeAll)
            _parent->logMessage(QString("BEFORE_ALL: %1").arg(msg));
        else if (_kind == TestKind::AfterAll)
            _parent->logMessage(QString("AFTER_ALL: %1").arg(msg));
        else _log.append(msg);
    }
    else _log.append(msg);
}

void TestBase::logMessage(const QStringList& list)
{
    for (auto s : list) logMessage(s);
}

} // namespace Tests
} // namespace Ori
