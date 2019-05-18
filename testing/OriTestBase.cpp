#include "OriTestBase.h"

#include <QDateTime>
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

TestSession::TestSession()
{
    if (TestLogger::enabled())
        TestLogger::reset();
    TEST_LOGGER(QString("*************** SESSION ***************\n"
                        "Started at %1\n")
                      .arg(QDateTime::currentDateTime().toString()));

    _testsRun = 0;
    _testsFail = 0;
    _testsPass = 0;
}

TestSession::~TestSession()
{
    TEST_LOGGER(QString("STAT: tests: %1, passed: %2, failed: %3\n"
                        "***************** END ******************\n")
                      .arg(_testsRun).arg(_testsPass).arg(_testsFail));
}

void TestSession::reset(const TestSuite &tests)
{
    for (auto test : tests) test->reset();
}

void TestSession::run(const TestSuite &tests)
{
    int count = tests.size();
    for (int i = 0; i < count; i++)
        run(tests.at(i), i == count-1);
}

void TestSession::run(TestBase *test, bool isLastInGroup)
{
    TestGroup *parentGroup = asGroup(test->parent());

    if (parentGroup)
    {
        // Group hasn't its own run method.
        // It's treated as run when its tests are run.
        parentGroup->_hasRun = true;
    }

    // Run BEFORE_ALL stage
    if (parentGroup)
    {
        auto beforeAll = parentGroup->_beforeAll;
        if (beforeAll)
        {
            if (!beforeAll->hasRun())
            {
                beforeAll->run();
                if (!beforeAll->result())
                {
                    parentGroup->setResult(false);
                    parentGroup->setMessage("BEFORE_ALL stage failed");
                    parentGroup->logMessage("BEFORE_ALL stage failed");
                }
                parentGroup->logMessage(beforeAll->log().trimmed());
            }
            // Don't run the test if `before_all` of its group failed
            if (!beforeAll->result()) return;
        }
    }

    // Run the test
    TestGroup *group = asGroup(test);
    if (group)
    {
        run(group->tests());
    }
    else
    {
        TEST_LOGGER(QString("**************** TEST ******************\n%1")
                        .arg(test->path()));

        _testsRun++;

        test->run();

        if (test->result())
        {
            _testsPass++;

            if (parentGroup)
                parentGroup->logMessage(QString("Passed: %1").arg(test->name()));

            TEST_LOGGER("*************** PASSED *****************\n");
        }
        else
        {
            _testsFail++;

            if (parentGroup)
            {
                parentGroup->setResult(false);
                if (parentGroup->message().isEmpty())
                    parentGroup->setMessage("Some of children tests failed");
                parentGroup->logMessage(QString("Failed: %1").arg(test->name()));
            }

            TEST_LOGGER("*************** FAILED *****************\n");
        }
    }

    // Run AFTER_ALL stage
    if (parentGroup and isLastInGroup)
    {
        auto afterAll = parentGroup->_afterAll;
        if (afterAll)
        {
            afterAll->run();
            if (!afterAll->result())
            {
                parentGroup->setResult(false);
                if (parentGroup->message().isEmpty())
                    parentGroup->setMessage("AFTER_ALL stage failed");
                parentGroup->logMessage("AFTER_ALL stage failed");
            }
            parentGroup->logMessage(afterAll->log().trimmed());
        }
    }
}

//------------------------------------------------------------------------------
//                                 TestGroup
//------------------------------------------------------------------------------

TestGroup::TestGroup(const char *name, std::initializer_list<TestBase*> args) : TestBase(name, nullptr)
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
    default: _tests.push_back(test);
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

void TestBase::setResult(bool value)
{
    _result = value;
}

void TestBase::reset()
{
    _hasRun = false;
    _result = true;
    _message.clear();
    _log.clear();
}

void TestBase::run()
{
    _hasRun = true;
    if (_method)
        _method(this);
}

void TestBase::logAssertion(const QString& assertion, const QString& condition,
                            const QString& expected, const QString& actual,
                            const QString &file, int line)
{
    logMessage(QString("Assertion : %1\n"
                       "Condition : %2\n"
                       "Expected  : %3\n"
                       "Actual    : %4\n"
                       "Location  : %5:%6\n")
                    .arg(assertion)
                    .arg(condition)
                    .arg(expected)
                    .arg(actual)
                    .arg(file)
                    .arg(line));
}

void TestBase::logMessage(const QString& msg)
{
    TestLogger::write(msg);
    _log += msg + "\n";
}

void TestBase::logMessage(const QStringList& list)
{
    foreach (const QString& s, list) logMessage(s);
}

} // namespace Tests
} // namespace Ori
