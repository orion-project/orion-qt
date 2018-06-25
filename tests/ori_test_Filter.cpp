#include "../testing/OriTestBase.h"
#include "../core/OriFilter.h"

namespace Ori {
namespace Tests {
namespace FilterTests {

class MockTestCondition {
public:
    QString id;
    Ori::Testing::TestBase* test;
    bool result;

    MockTestCondition(const QString& id, Ori::Testing::TestBase *test, bool result = true)
        : id(id), test(test), result(result) {}

    ~MockTestCondition()
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

TEST_METHOD(destructor_must_delete_all_conditions)
{
    const int conditionCount = 5;
    auto filter = new Filter<QString, MockTestCondition>;
    for (int i = 0; i < conditionCount; i++)
        filter->append(new MockTestCondition(QString::number(i), test));

    // when
    delete filter;

    // then
    for (int i = 0; i < conditionCount; i++)
        ASSERT_EQ_STR(test->data(QString::number(i)).toString(), "deleted")
}

TEST_METHOD(check_must_call_all_conditions)
{
    Filter<QString, MockTestCondition> filter({
        new MockTestCondition("1", test),
        new MockTestCondition("2", test)
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
    Filter<QString, MockTestCondition> filter({
        new MockTestCondition("1", test, true),
        new MockTestCondition("2", test, false)
    });

    // when
    bool result = filter.check("");

    // then
    ASSERT_IS_FALSE(result)
}

class IntTestCondition
{
public:
    IntTestCondition(int ref): _ref(ref) {}
    bool check(int val) const { return val == _ref; }
private:
    int _ref;
};

TEST_METHOD(count_must_return_number_of_items_in_container)
{
    Filter<int, IntTestCondition> filter({
        new IntTestCondition(100)
    });

    std::vector<int> vals({100, 100, 200, 300});

    // when
    int result = filter.count(vals);

    // then
    ASSERT_EQ_INT(result, 2)
}

TEST_METHOD(count_must_return_zero_when_no_conditions_satisfied)
{
    Filter<int, IntTestCondition> filter({
        new IntTestCondition(100)
    });

    std::vector<int> vals({400, 400, 200, 300});

    // when
    int result = filter.count(vals);

    // then
    ASSERT_EQ_INT(result, 0)
}

TEST_METHOD(filter_must_return_part_of_container)
{
    Filter<int, IntTestCondition> filter({
        new IntTestCondition(100)
    });

    std::vector<int> vals({100, 100, 200, 300});

    // when
    auto result = filter.filter(vals);

    // then
    ASSERT_EQ_INT(result.size(), 2)
    ASSERT_EQ_INT(result[0], 100)
    ASSERT_EQ_INT(result[1], 100)
}

TEST_METHOD(filter_must_return_empty_container_when_no_conditions_satisfied)
{
    Filter<int, IntTestCondition> filter({
        new IntTestCondition(100)
    });

    std::vector<int> vals({400, 400, 200, 300});

    // when
    auto result = filter.filter(vals);

    // then
    ASSERT_IS_TRUE(result.empty())
}

//------------------------------------------------------------------------------

TEST_GROUP("Filter",
    ADD_TEST(destructor_must_delete_all_conditions),
    ADD_TEST(check_must_call_all_conditions),
    ADD_TEST(check_must_return_false_when_one_condition_failed),
    ADD_TEST(count_must_return_number_of_items_in_container),
    ADD_TEST(count_must_return_zero_when_no_conditions_satisfied),
    ADD_TEST(filter_must_return_part_of_container),
    ADD_TEST(filter_must_return_empty_container_when_no_conditions_satisfied),
)

} // namespace TemplatesTests
} // namespace Tests
} // namespace Ori
