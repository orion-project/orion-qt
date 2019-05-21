#include "OriTestBase.h"

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QStringList>
#include <QTextStream>

namespace Ori {
namespace Testing {

TestGroup* asGroup(TestBase* test)
{
    return dynamic_cast<TestGroup*>(test);
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
                        "***************** END ******************\n")
                      .arg(_testsRun).arg(_testsPass).arg(_testsFail));
}

void TestSession::run()
{
    _testsRun = 0;
    _testsFail = 0;
    _testsPass = 0;

    for (auto test : _tests)
        test->reset();

    runGroup(_tests);

    if (emitSignals) emit sessionFinished();
}

void TestSession::runGroup(const TestSuite& tests)
{
    int count = tests.size();
    for (int i = 0; i < count; i++)
        runTest(tests.at(i), i == count-1);
}

void TestSession::runTest(TestBase *test, bool isLastInGroup)
{
    TestGroup *parentGroup = asGroup(test->parent());

    if (parentGroup)
    {
        auto beforeAll = parentGroup->_beforeAll;
        if (beforeAll)
        {
            if (beforeAll->result() == TestResult::None) beforeAll->runTest();
            if (beforeAll->result() != TestResult::Pass) return;
        }
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
        test->runTest();

        switch (test->result())
        {
        case TestResult::Pass: _testsPass++; break;
        case TestResult::Fail: _testsFail++; break;
        default: break;
        }

        notifyTestFinished(test);
    }

    if (parentGroup and isLastInGroup and parentGroup->_afterAll)
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
            case TestKind::BeforeAll:
                _parent->logMessage(QStringLiteral("FAIL: BEFORE_ALL"));
                break;
            case TestKind::AfterAll:
                _parent->logMessage(QStringLiteral("FAIL: AFTER_ALL"));
                break;
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
    _result = TestResult::None;
    _message.clear();
    _log.clear();
}

void TestBase::runTest()
{
    run();

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
    auto msg = QStringLiteral
                      ("Assertion : %1\n"
                       "Condition : %2\n"
                       "Expected  : %3\n"
                       "Actual    : %4\n"
                       "Location  : %5:%6\n")
                    .arg(assertion)
                    .arg(condition)
                    .arg(expected)
                    .arg(actual)
                    .arg(file)
                    .arg(line);

    if (_parent)
    {
        if (_kind == TestKind::BeforeAll)
            _parent->logMessage(QString("BEFORE_ALL: Assertion:\n%2").arg(msg));
        else if (_kind == TestKind::AfterAll)
            _parent->logMessage(QString("AFTER_ALL: Assertion:\n%2").arg(msg));
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
            _parent->logMessage(QString("BEFORE_ALL: %2").arg(msg));
        else if (_kind == TestKind::AfterAll)
            _parent->logMessage(QString("AFTER_ALL: %2").arg(msg));
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
