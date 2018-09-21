#include "OriTestBase.h"

#include <QDateTime>
#include <QFile>
#include <QStringList>
#include <QTextStream>

namespace Ori {
namespace Testing {

void free(const TestSuite& tests)
{
    for (auto t : tests) delete t;
}

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
    if (!enabled()) return;

    if (QFile::exists(fileName()))
        QFile::remove(fileName());
}

void TestLogger::write(const QString& msg)
{
    if (!enabled()) return;

    QFile file(fileName());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return;

    QTextStream out(&file);
    out << msg << "\n";

    file.close();
}

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
    TestLogger::reset();
    TestLogger::write(QString("*************** SESSION ***************\nStarted at %1\n")
                      .arg(QDateTime::currentDateTime().toString()));

    _testsRun = 0;
    _testsFail = 0;
    _testsPass = 0;
}

TestSession::~TestSession()
{
    TestLogger::write(QString("STAT: tests: %1, passed: %2, failed: %3\n"
                              "***************** END ******************")
                      .arg(_testsRun).arg(_testsPass).arg(_testsFail));
}

void TestSession::run(const TestSuite &tests)
{
    foreach (TestBase *test, tests)
        run(test);
}

void TestSession::run(TestBase *test)
{
    test->reset();

    TestGroup *group = dynamic_cast<TestGroup*>(test);
    if (!group)
    {
        TestLogger::write(QString("**************** TEST ******************\n%1")
                          .arg(test->path()));

        _testsRun++;

        test->run();

        if (test->result())
            _testsPass++;
        else
            _testsFail++;

        TestLogger::write(QString("*************** %1 *****************\n")
                          .arg(test->result()? "PASSED": "FAILED"));
    }
    else
        run(group->tests());
}

//------------------------------------------------------------------------------
//                                 TestGroup
//------------------------------------------------------------------------------

TestGroup::TestGroup(const char *name, std::initializer_list<TestBase*> args) : TestGroup(name)
{
    for (auto arg : args) append(arg);
}

TestGroup* TestGroup::create(const char *name, ...)
{
    TestGroup *group = new TestGroup(name);

    TestBase *test;
    va_list tests;
    va_start(tests, name);
    test = va_arg(tests, TestBase*);
    while (test)
    {
        group->append(test);
        test = va_arg(tests, TestBase*);
    }
    va_end(tests);

    return group;
}

void TestGroup::append(TestBase *test)
{
    _tests << test;
    test->_parent = this;
}

void TestGroup::run()
{
    reset();
    foreach (TestBase *test, _tests)
    {
        test->reset();
        test->run();
    }
}

//------------------------------------------------------------------------------
//                                 TestBase
//------------------------------------------------------------------------------

TestBase::TestBase(const char *name, TestMethod method)
{
    _result = true;
    _parent = nullptr;
    _name = name;
    _method = method;
}

TestBase::~TestBase()
{
}

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
    if (_parent)
        _parent->setResult(_parent->result() && value);
}

void TestBase::reset()
{
    _result = true;
    _message.clear();
    _log.clear();
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
