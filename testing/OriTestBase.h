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

#define ASSERT_IS_NULL(expr_ptr) { \
    auto __test_var_ptr__ = reinterpret_cast<const void*>(expr_ptr); \
    if (__test_var_ptr__ != nullptr) \
    { \
        test->setResult(false); \
        test->setMessage("Some value is assigned"); \
        test->logAssertion("IS NULL", #expr_ptr, "NULL", formatPtr(__test_var_ptr__), __FILE__, __LINE__); \
        return; \
    }}

#define ASSERT_IS_NOT_NULL(expr_ptr) { \
    auto __test_var_ptr__ = reinterpret_cast<const void*>(expr_ptr); \
    if (__test_var_ptr__ == nullptr) \
    { \
        test->setResult(false); \
        test->setMessage("Value is not assigned"); \
        test->logAssertion("IS NOT NULL", #expr_ptr, "NOT NULL", "NULL", __FILE__, __LINE__); \
        return; \
    }}

#define ASSERT_IS_TYPE(obj, type) {                                                       \
    if (dynamic_cast<type*>(obj) == nullptr)                                              \
    {                                                                                     \
        test->setResult(false);                                                           \
        test->setMessage("Type of value is not expected");                                \
        test->logAssertion("IS TYPE", #type, "TRUE", "FALSE", __FILE__, __LINE__);        \
        return;                                                                           \
    }}

#define ASSERT_EQ_STR(expr_value, expr_expected) { \
    QString __test_var_value__(expr_value); \
    QString __test_var_expected__(expr_expected); \
    if (__test_var_value__ != __test_var_expected__) \
    { \
        test->setResult(false); \
        test->setMessage("Value is not equal to expected" ); \
        test->logAssertion("ARE STRINGS EQUAL", \
                           QString("%1 == %2").arg(#expr_value).arg(#expr_expected), \
                           QString(__test_var_expected__), \
                           QString(__test_var_value__), __FILE__, __LINE__); \
        return; \
    }}

#define ASSERT_EQ_PTR(expr_value, expr_expected) { \
    auto __test_var_value__ = reinterpret_cast<const void*>(expr_value); \
    auto __test_var_expected__ = reinterpret_cast<const void*>(expr_expected); \
    if (__test_var_value__ != __test_var_expected__) \
    { \
        test->setResult(false); \
        test->setMessage("Pointer is not equal to expected" ); \
        test->logAssertion("ARE POINTERS EQUAL", \
                           QString("%1 == %2").arg(#expr_value).arg(#expr_expected), \
                           formatPtr(__test_var_expected__), \
                           formatPtr(__test_var_value__), \
                           __FILE__, __LINE__); \
        return; \
    }}

#define ASSERT_NEQ_PTR(expr_value, expr_expected) { \
    auto __test_var_value__ = reinterpret_cast<const void*>(expr_value); \
    auto __test_var_expected__ = reinterpret_cast<const void*>(expr_expected); \
    if (__test_var_value__ == __test_var_expected__) \
    { \
        test->setResult(false); \
        test->setMessage("Pointers are equal" ); \
        test->logAssertion("ARE NOT POINTERS EQUAL", \
                           QString("%1 == %2").arg(#expr_value).arg(#expr_expected), \
                           formatPtr(__test_var_expected__), \
                           formatPtr(__test_var_value__), \
                           __FILE__, __LINE__); \
        return; \
    }}

#define ASSERT_EQ_INT(expr_value, expr_expected) { \
    int __test_var_value__ = int(expr_value); \
    int __test_var_expected__ = int(expr_expected); \
    if (__test_var_value__ != __test_var_expected__) \
    { \
        test->setResult(false); \
        test->setMessage("Value is not equal to expected" ); \
        test->logAssertion("ARE INTEGERS EQUAL", \
                           QString("%1 == %2").arg(#expr_value).arg(#expr_expected), \
                           QString::number(__test_var_expected__), \
                           QString::number(__test_var_value__), \
                           __FILE__, __LINE__); \
        return; \
    }}

#define ASSERT_EQ_DBL(expr_value, expr_expected) { \
    double __test_var_value__ = double(expr_value); \
    double __test_var_expected__ = double(expr_expected); \
    if (!SAME_DOUBLE(__test_var_value__, __test_var_expected__)) \
    { \
        test->setResult(false); \
        test->setMessage("Value is not equal to expected" ); \
        test->logAssertion("ARE DOUBLES EQUAL", \
                           QString("%1 == %2").arg(#expr_value).arg(#expr_expected), \
                           QString::number(__test_var_expected__, 'g', 16), \
                           QString::number(__test_var_value__, 'g', 16), \
                           __FILE__, __LINE__); \
        return; \
    }}

#define ASSERT_NEAR_DBL(expr_value, expr_expected, epsilon) { \
    double __test_var_value__ = double(expr_value); \
    double __test_var_expected__ = double(expr_expected); \
    double __test_var_delta__ = qAbs(__test_var_value__ - __test_var_expected__); \
    if (std::isinf(__test_var_value__) || std::isnan(__test_var_value__) || __test_var_delta__ > epsilon) \
    { \
        test->setResult(false); \
        test->setMessage("Value is not equal to expected" ); \
        test->logAssertion("ARE DOUBLES NEAR EQUAL", \
                           QString("%1 == %2").arg(#expr_value).arg(#expr_expected), \
                           QString::number(__test_var_expected__, 'g', 16), \
                           QString::number(__test_var_value__, 'g', 16), \
                           __FILE__, __LINE__); \
        return; \
    }}

#define ASSERT_NEQ_DBL(expr_value, expr_expected) { \
    double __test_var_value__ = double(expr_value); \
    double __test_var_expected__ = double(expr_expected); \
    if (SAME_DOUBLE(__test_var_value__, __test_var_expected__)) \
    { \
        test->setResult(false); \
        test->setMessage("Values are equal" ); \
        test->logAssertion("ARE DOUBLES NOT EQUAL", \
                           QString("%1 != %2").arg(#expr_value).arg(#expr_expected), \
                           QString("Any value other than %1") \
                                .arg(__test_var_expected__, 0, 'g', 16), \
                           QString::number(__test_var_value__, 'g', 16), \
                           __FILE__, __LINE__); \
        return; \
    }}

#define ASSERT_EQ_DATA(key_name, expr_expected) { \
    QString __data_key__(key_name); \
    if (!test->data().contains(__data_key__)) \
    { \
        test->setResult(false); \
        test->setMessage("Data key not found"); \
        test->logAssertion("HAS DATA KEY", \
                           QString("test->data().contains(%1)").arg(#key_name), \
                           "TRUE", "FALSE", __FILE__, __LINE__); \
        return; \
    } \
    QVariant __test_var_value__ = test->data()[__data_key__]; \
    QVariant __test_var_expected__(expr_expected); \
    if (__test_var_value__ != __test_var_expected__) \
    { \
        test->setResult(false); \
        test->setMessage("Data is not equal to expected"); \
        test->logAssertion("ARE VARIANTS EQUAL",  \
                           QString("data[%1] == %2").arg(#key_name).arg(#expr_expected), \
                           __test_var_expected__.toString(), \
                           __test_var_value__.toString(), \
                           __FILE__, __LINE__); \
        return; \
    } \
}

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
    void name(Ori::Testing::TestBase *test)

#define TEST_CASE_METHOD(name, ...)                                                       \
    void name(Ori::Testing::TestBase *test, __VA_ARGS__)

#define TEST_CASE(name, method, ...)                                                      \
    void name(Ori::Testing::TestBase *test)                                               \
    {                                                                                     \
        method(test, __VA_ARGS__);                                                        \
    }


#define TEST_GROUP(name, ...)                                                             \
    Ori::Testing::TestGroup* tests()                                                      \
    {                                                                                     \
        return new Ori::Testing::TestGroup(name, { __VA_ARGS__ });                        \
    }

#define TEST_SUITE(...)                                                                   \
    Ori::Testing::TestSuite tests()                                                       \
    {                                                                                     \
        return {__VA_ARGS__};                                                             \
    }

#define USE_GROUP(name)                                                                   \
    namespace name { Ori::Testing::TestGroup* tests(); }

#define USE_SUITE(name)                                                                   \
    namespace name { Ori::Testing::TestSuite tests(); }

#define ADD_SUITE(name)                                                                   \
    name::tests()

#define ADD_TEST(method)                                                                  \
    new Ori::Testing::TestBase(#method, method)

#define BEFORE_ALL(method)                                                                \
    new Ori::Testing::TestBase(#method, method, Ori::Testing::TestKind::BeforeAll)

#define AFTER_ALL(method)                                                                 \
    new Ori::Testing::TestBase(#method, method, Ori::Testing::TestKind::AfterAll)

#define BEFORE_EACH(method)                                                               \
    new Ori::Testing::TestBase(#method, method, Ori::Testing::TestKind::BeforeEach)

#define AFTER_EACH(method)                                                                \
    new Ori::Testing::TestBase(#method, method, Ori::Testing::TestKind::AfterEach)

#define ADD_GROUP(name)                                                                   \
    name::tests()

#define ADD_CLASS(name)                                                                   \
    new name()

#define TEST_CLASS(name)                                                                  \
    class name : public Ori::Testing::TestBase                                            \
    {                                                                                     \
    public:                                                                               \
        name() : Ori::Testing::TestBase(#name, nullptr) {}

#define TEST_CLASS_END                                                                    \
    };

#define TEST_LOG(s) test->logMessage(s);
#define TEST_LOG_VALUE(v) test->logMessage(QString("%1 = %2").arg(#v).arg(v));
#define TEST_LOG_DOUBLE(v) test->logMessage(QString("%1 = %2").arg(#v).arg(v, 0, 'g', 16));
#define TEST_LOG_PTR(p) test->logMessage(QString("%1 = 0x%2").arg(#p).arg(quintptr(p), 0, 16));
#define TEST_LOG_SEPARATOR test->logMessage("---------------------------------------");
#define SET_TEST_DATA(key, value) test->data().insert(key, value)

namespace Ori {
namespace Testing {

class TestBase;
class TestGroup;
class TestSession;

typedef QVector<TestBase*> TestSuite;

TestGroup* asGroup(TestBase* test);


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


typedef void (*TestMethod) (TestBase *test);

enum class TestKind {Test, Group, BeforeAll, AfterAll, BeforeEach, AfterEach};
enum class TestResult {None, Pass, Fail};

class TestBase
{
public:
    TestBase(const char *name, TestMethod method, TestKind kind = TestKind::Test);
    virtual ~TestBase();

    QString name() const { return _name; }
    void runTest();
    virtual void reset();
    TestResult result() const { return _result; }
    void setResult(bool pass);
    QString message() const { return _message; }
    void setMessage(const QString& msg);
    QStringList log() const { return _log; }
    QString path() const;
    TestBase* parent() const { return _parent; }
    QMap<QString, QVariant>& data() { return _data; }
    QVariant& data(const QString& key) { return _data[key]; }
    TestKind kind() const { return _kind; }
    void logMessage(const QString& msg);
    void logMessage(const QStringList& list);
    void logAssertion(const QString& assertion, const QString& condition,
                      const QString& expected, const QString& actual,
                      const QString& file, int line);
protected:
    virtual void run();
private:
    TestResult _result = TestResult::None;
    QString _name;
    QString _message;
    QStringList _log;
    TestBase *_parent = nullptr;
    TestMethod _method = nullptr;
    QMap<QString, QVariant> _data;
    TestKind _kind = TestKind::Test;

    friend class TestGroup;
    friend class TestSession;
};

class TestGroup : public TestBase
{
public:
    TestGroup(const char *name, std::initializer_list<TestBase*> args);
    ~TestGroup() override { qDeleteAll(_tests); }

    const TestSuite& tests() const { return _tests; }
    void reset() override;

    void append(TestBase *test);

private:
    TestSuite _tests;
    TestBase* _beforeAll = nullptr;
    TestBase* _afterAll = nullptr;
    TestBase* _beforeEach = nullptr;
    TestBase* _afterEach = nullptr;

    friend class TestSession;
};


class TestSession : public QObject
{
    Q_OBJECT

public:
    TestSession(const TestSuite& tests);
    ~TestSession();

    void run();

    int testsCount() const { return countTestsInGroup(_tests); }
    int testsRun() const { return _testsRun; }
    int testsPass() const { return _testsPass; }
    int testsFail() const { return _testsFail; }

    bool emitSignals = false;

signals:
    void testRunning(TestBase* test);
    void testFinished(TestBase* test);
    void sessionFinished();

private:
    TestSuite _tests;
    int _testsRun = 0;
    int _testsPass = 0;
    int _testsFail = 0;

    void runGroup(const TestSuite& tests);
    int countTestsInGroup(const TestSuite& tests) const;
    void runTest(TestBase *test, bool isLastInGroup);
    void notifyTestRunning(TestBase* test);
    void notifyTestFinished(TestBase* test);
};

} // namespace Testing
} // namespace Ori

#endif // ORI_TEST_BASE_H
