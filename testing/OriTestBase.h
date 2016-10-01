#ifndef ORI_TEST_BASE_H
#define ORI_TEST_BASE_H

/**
// Tests can be combined into named groups.
// Each test group is a separate namespace and can be placed into separate cpp-file.
namespace MyTestGroup1 {

    // Define test methods
    TEST_METHOD(test1) { ... }
    TEST_METHOD(test2) { ... }

    // Define named test group
    TEST_GROUP("Test something",
        ADD_TEST(test1),
        ADD_TEST(test2),
    )
}

// Test suite is collection of individual tests or test groups.
// Attach groups if they was defined in separate cpp-files.
USE_GROUP(MyTestGroup1)
USE_GROUP(MyTestGroup2)

namespace MyTestSuite {
    TEST_SUITE(
        ADD_GROUP(MyTestGroup1),
        ADD_GROUP(MyTestGroup2),
    )
}

// main.cpp
#include "TestManager.h"

// Attach suites if they was defined in separate cpp-files.
USE_SUITE(MyTestSuite)

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Check is application has been started with 'test' command-line argument
    // QApplication must be initialized before, to command-line arguments be processed.
    if (Ori::Test::isTesting())

        // Run application in testing mode.
        return Ori::Test::run(app, {
            ADD_SUITE(MyTestSuite)
        });
}
**/

#include <QMap>
#include <QString>
#include <QVariant>
#include <QVector>

#include "../core/OriFloatingPoint.h"

inline QString formatPtr(const void* ptr)
{
    return QString("0x%1").arg(quintptr(ptr), 0, 16);
}

#define ASSERT_IS_TRUE(condition) {                                                       \
    if (!(condition))                                                                     \
    {                                                                                     \
        test->setResult(false);                                                           \
        test->setMessage("Condition is false: " # condition);                             \
        test->logAssertion("IS TRUE", #condition, "TRUE", "FALSE", __FILE__, __LINE__);   \
        return;                                                                           \
    }}

#define ASSERT_IS_FALSE(condition) {                                                      \
    if (condition)                                                                        \
    {                                                                                     \
        test->setResult(false);                                                           \
        test->setMessage("Condition is true: " # condition);                              \
        test->logAssertion("IS FALSE", #condition, "FALSE", "TRUE", __FILE__, __LINE__);  \
        return;                                                                           \
    }}

#define ASSERT_IS_NULL(ptr) {                                                             \
    if ((void*)(ptr) != nullptr)                                                          \
    {                                                                                     \
        test->setResult(false);                                                           \
        test->setMessage("Some value is assigned");                                       \
        test->logAssertion("IS NULL", #ptr, "NULL", formatPtr(ptr), __FILE__, __LINE__);  \
        return;                                                                           \
    }}

#define ASSERT_IS_NOT_NULL(ptr) {                                                         \
    if ((void*)(ptr) == nullptr)                                                          \
    {                                                                                     \
        test->setResult(false);                                                           \
        test->setMessage("Value is not assigned");                                        \
        test->logAssertion("IS NOT NULL", #ptr, "NOT NULL", "NULL", __FILE__, __LINE__);  \
        return;                                                                           \
    }}

#define ASSERT_IS_TYPE(obj, type) {                                                       \
    if (dynamic_cast<type*>(obj) == nullptr)                                              \
    {                                                                                     \
        test->setResult(false);                                                           \
        test->setMessage("Type of value is not expected");                                \
        test->logAssertion("IS TYPE", #type, "TRUE", "FALSE", __FILE__, __LINE__);        \
        return;                                                                           \
    }}

#define ASSERT_EQ_STR(value, expected) {                                                  \
    if (QString(value) != QString(expected))                                              \
    {                                                                                     \
        test->setResult(false);                                                           \
        test->setMessage("Value is not equal to expected" );                              \
        test->logAssertion("ARE STRINGS EQUAL",                                           \
                           QString("%1 == %2").arg(#value).arg(#expected),                \
                           QString(expected),                                             \
                           QString(value), __FILE__, __LINE__);                           \
        return;                                                                           \
    }}

#define ASSERT_EQ_PTR(value, expected) {                                                  \
    if ((value) != (expected))                                                            \
    {                                                                                     \
        test->setResult(false);                                                           \
        test->setMessage("Pointer is not equal to expected" );                            \
        test->logAssertion("ARE POINTERS EQUAL",                                          \
                           QString("%1 == %2").arg(#value).arg(#expected),                \
                           formatPtr(expected),                                           \
                           formatPtr(value),                                              \
                           __FILE__, __LINE__);                                           \
        return;                                                                           \
    }}

#define ASSERT_EQ_INT(value, expected) {                                                  \
    if ((value) != (expected))                                                            \
    {                                                                                     \
        test->setResult(false);                                                           \
        test->setMessage("Value is not equal to expected" );                              \
        test->logAssertion("ARE INTEGERS EQUAL",                                          \
                           QString("%1 == %2").arg(#value).arg(#expected),                \
                           QString::number(expected),                                     \
                           QString::number(value),                                        \
                           __FILE__, __LINE__);                                           \
        return;                                                                           \
    }}

#define ASSERT_EQ_DBL(value, expected) {                                                  \
    if (!SAME_DOUBLE((value), (expected)))                                                \
    {                                                                                     \
        test->setResult(false);                                                           \
        test->setMessage("Value is not equal to expected" );                              \
        test->logAssertion("ARE DOUBLES EQUAL",                                           \
                           QString("%1 == %2").arg(#value).arg(#expected),                \
                           QString::number(double(expected), 'g', 16),                    \
                           QString::number(double(value), 'g', 16),                       \
                           __FILE__, __LINE__);                                           \
        return;                                                                           \
    }}

#define ASSERT_NEQ_DBL(value, expected) {                                                 \
    if (SAME_DOUBLE((value), (expected)))                                                 \
    {                                                                                     \
        test->setResult(false);                                                           \
        test->setMessage("Values are equal" );                                            \
        test->logAssertion("ARE DOUBLES NOT EQUAL",                                       \
                           QString("%1 != %2").arg(#value).arg(#expected),                \
                           QString("Any value other than %1")                             \
                                .arg(double(expected), 0, 'g', 16),                       \
                           QString::number(double(value), 'g', 16),                       \
                           __FILE__, __LINE__);                                           \
        return;                                                                           \
    }}

#define ASSERT_FAIL(msg) {                                                                \
    test->setResult(false);                                                               \
    test->setMessage(msg);                                                                \
    test->logMessage(msg + QString("\nLocation: %2:%3").arg(__FILE__).arg(__LINE__));     \
    return;                                                                               \
    }

#define ASSERT_OK(msg) {                                                                  \
    test->setResult(true);                                                                \
    test->setMessage(msg);                                                                \
    test->logMessage(msg + QString("\nLocation: %2:%3").arg(__FILE__).arg(__LINE__));     \
    return;                                                                               \
    }


#define TEST_METHOD(name)                                                                 \
    void name(Ori::Test::TestBase *test)

#define TEST_GROUP(name, ...)                                                             \
    Ori::Test::TestGroup* tests()                                                         \
    {                                                                                     \
        return new Ori::Test::TestGroup(name, { __VA_ARGS__ });                           \
    }

#define TEST_SUITE(...)                                                                   \
    Ori::Test::TestSuite tests()                                                          \
    {                                                                                     \
        return {__VA_ARGS__};                                                             \
    }

#define USE_GROUP(name)                                                                   \
    namespace name { Ori::Test::TestGroup* tests(); }

#define USE_SUITE(name)                                                                   \
    namespace name { Ori::Test::TestSuite tests(); }

#define ADD_SUITE(name)                                                                   \
    name::tests()

#define ADD_TEST(method)                                                                  \
    new Ori::Test::TestBase(#method, method)

#define ADD_GROUP(name)                                                                   \
    name::tests()

#define ADD_CLASS(name)                                                                   \
    new name()

#define TEST_CLASS(name)                                                                  \
    class name : public Ori::Test::TestBase                                               \
    {                                                                                     \
    public:                                                                               \
        name() : Ori::Test::TestBase(#name, nullptr) {}

#define TEST_CLASS_END                                                                    \
    };

#define TEST_LOG(s) test->logMessage(s);
#define TEST_LOG_VALUE(v) test->logMessage(QString("%1 = %2").arg(#v).arg(v));
#define TEST_LOG_DOUBLE(v) test->logMessage(QString("%1 = %2").arg(#v).arg(v, 0, 'g', 16));
#define TEST_LOG_PTR(p) test->logMessage(QString("%1 = 0x%2").arg(#p).arg(quintptr(p), 0, 16));
#define TEST_LOG_SEPARATOR test->logMessage("---------------------------------------");


namespace Ori {
namespace Test {

class TestBase;
class TestGroup;
class TestSession;

typedef QVector<TestBase*> TestSuite;

void free(const TestSuite& tests);
TestGroup* asGroup(TestBase* test);

////////////////////////////////////////////////////////////////////////////////

class TestLogger
{
public:
    static void reset();
    static void write(const QString& msg);
    static QString fileName() { return "test.log"; } // TODO: check on Linux
    static bool enabled() { return _enabled; }
    static void enable(bool value) { _enabled = value; }

private:
    static bool _enabled;
};

////////////////////////////////////////////////////////////////////////////////

typedef void (*TestMethod) (TestBase *test);

////////////////////////////////////////////////////////////////////////////////

class TestBase
{
public:
    TestBase(const char *name, TestMethod method);
    virtual ~TestBase() {}

    const char* name() const { return _name; }
    virtual void run() { if (_method) _method(this); }
    void reset();
    bool result() const { return _result; }
    void setResult(bool value);
    QString message() const { return _message; }
    void setMessage(const QString& msg) { _message = msg; }
    QString log() const { return _log; }
    QString path() const;
    TestBase* parent() const { return _parent; }
    QMap<QString, QVariant>& data() { return _data; }
    void logMessage(const QString& msg);
    void logMessage(const QStringList& list);
    void logAssertion(const QString& assertion, const QString& condition,
                      const QString& expected, const QString& actual,
                      const QString& file, int line);
private:
    bool _result;
    const char *_name;
    QString _message;
    QString _log;
    TestBase *_parent;
    TestMethod _method;
    QMap<QString, QVariant> _data;

    friend class TestGroup;
};

////////////////////////////////////////////////////////////////////////////////

class TestGroup : public TestBase
{
public:
    TestGroup(const char *name) : TestBase(name, nullptr) {}
#ifdef Q_COMPILER_INITIALIZER_LISTS
    TestGroup(const char *name, std::initializer_list<TestBase*> args);
#endif
    ~TestGroup() { free(_tests); }

    static TestGroup* create(const char *name, ...);

    const TestSuite& tests() const { return _tests; }
    void append(TestBase *test);
    void run();

private:
    TestSuite _tests;
};

////////////////////////////////////////////////////////////////////////////////

class TestSession
{
public:
    TestSession();
    ~TestSession();

    void run(const TestSuite& tests);
    void run(TestBase *test);
    int testsRun() const { return _testsRun; }
    int testsPass() const { return _testsPass; }
    int testsFail() const { return _testsFail; }

private:
    int _testsRun, _testsPass, _testsFail;
};

////////////////////////////////////////////////////////////////////////////////

} // namespace Test
} // namespace Ori

#endif // ORI_TEST_BASE_H
