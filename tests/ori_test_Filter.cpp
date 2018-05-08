#include "../testing/OriTestBase.h"
#include "../core/OriFilter.h"

namespace Ori {
namespace Tests {
namespace FilterTests {

class TestCondition {
public:
    QString id;
    Ori::Testing::TestBase* test;
    bool result;

    TestCondition(const QString& id, Ori::Testing::TestBase *test, bool result = true)
        : id(id), test(test), result(result) {}

    ~TestCondition()
    {
        TEST_LOG("~TestCondition() " + id)
        test->data().insert(id, "deleted");
    }

    bool check(const QString& s)
    {
        TEST_LOG("check() " + id)
        test->data().insert(id, QString("checked %1").arg(s));
        return result;
    }
};

typedef Filter<QString, TestCondition> TestFilter;

TEST_METHOD(destructor_must_delete_all_conditions)
{
    const int conditionCount = 5;
    auto filter = new TestFilter;
    for (int i = 0; i < conditionCount; i++)
        filter->append(new TestCondition(QString::number(i), test));

    // when
    delete filter;

    // then
    for (int i = 0; i < conditionCount; i++)
        ASSERT_EQ_STR(test->data(QString::number(i)).toString(), "deleted")
}

TEST_METHOD(check_must_call_all_conditions)
{
    TestFilter filter({
        new TestCondition("1", test),
        new TestCondition("2", test)
    });
    const QString checkValue("test");

    // when
    bool result = filter.check(checkValue);

    // then
    ASSERT_IS_TRUE(result)
    ASSERT_EQ_STR(test->data("1").toString(), "checked " + checkValue)
    ASSERT_EQ_STR(test->data("2").toString(), "checked " + checkValue)
}

TEST_METHOD(check_must_return_false_when_one_condition_failed)
{
    TestFilter filter({
        new TestCondition("1", test, true),
        new TestCondition("2", test, false)
    });

    // when
    bool result = filter.check("");

    // then
    ASSERT_IS_FALSE(result)
}

//------------------------------------------------------------------------------

TEST_GROUP("Filter",
    ADD_TEST(destructor_must_delete_all_conditions),
    ADD_TEST(check_must_call_all_conditions),
    ADD_TEST(check_must_return_false_when_one_condition_failed)
)

} // namespace TemplatesTests
} // namespace Tests
} // namespace Ori
