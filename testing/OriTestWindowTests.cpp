#include "OriTestBase.h"

#include <QMutex>
#include <QVector>
#include <QWaitCondition>

using namespace Ori::Testing;

/// Tests for testing the test window
namespace TestWindowTests {

// Causes the current thread to sleep for msecs milliseconds.
void sleep(unsigned long msecs)
{
    QMutex mutex;
    mutex.lock();

    QWaitCondition waitCondition;
    waitCondition.wait(&mutex, msecs);

    mutex.unlock();
}

TEST_METHOD(TestSuccessed)
{
    sleep(100);
    ASSERT_OK("OK")
}

TEST_METHOD(TestFailed)
{
    sleep(100);
    ASSERT_IS_TRUE(false);
}

TEST_METHOD(TestAssertIsTruePassed)
{
    sleep(100);
    ASSERT_IS_TRUE(2+2 == 4)
}

TEST_METHOD(TestAssertIsTrueFailed)
{
    sleep(100);
    ASSERT_IS_TRUE(2+2 == 5)
}

TEST_METHOD(TestAssertIsFalsePassed)
{
    sleep(100);
    ASSERT_IS_FALSE(2 == 3)
}

TEST_METHOD(TestAssertIsFalseFailed)
{
    sleep(100);
    ASSERT_IS_FALSE(2 != 3)
}

TEST_METHOD(RunSuccessfulGroup)
{
    TestGroup *group = new TestGroup("", {
        ADD_TEST(TestSuccessed),
        ADD_TEST(TestSuccessed) });
    group->run();
    ASSERT_IS_TRUE(group->result());
}

TEST_METHOD(RunSuccessfulCompoundGroup)
{
    TestGroup *innerGroup = new TestGroup("", {
        ADD_TEST(TestSuccessed),
        ADD_TEST(TestSuccessed) });

    TestGroup *group = new TestGroup("", {
        ADD_TEST(TestSuccessed),
        ADD_TEST(TestSuccessed) });
    group->append(innerGroup);

    group->run();
    ASSERT_IS_TRUE(group->result());
}

TEST_METHOD(RunFailureGroup)
{
    TestGroup *group = new TestGroup("", {
        ADD_TEST(TestSuccessed),
        ADD_TEST(TestFailed) });
    group->run();
    ASSERT_IS_TRUE(group->result());
}

TEST_METHOD(RunFailureCompoundGroup)
{
    TestGroup *innerGroup = new TestGroup("");
    innerGroup->append(ADD_TEST(TestSuccessed));
    innerGroup->append(ADD_TEST(TestFailed));

    TestGroup *group = new TestGroup("");
    group->append(ADD_TEST(TestSuccessed));
    group->append(ADD_TEST(TestSuccessed));
    group->append(innerGroup);

    group->run();
    ASSERT_IS_TRUE(group->result());
}

TEST_CLASS(TestClassSuccessed)
    void run()
    {
        TestBase *test = this;
        ASSERT_IS_TRUE(2 == 2);
    }
TEST_CLASS_END

TEST_CLASS(TestClassFailed)
    void run()
    {
        TestBase *test = this;
        ASSERT_IS_TRUE(2 == 3);
    }
TEST_CLASS_END

namespace GroupSuccessed {
    TEST_GROUP("Successed Group",
        ADD_TEST(TestSuccessed),
        ADD_TEST(TestSuccessed),
    )
}

namespace GroupFailed {
    TEST_GROUP("Failed Group",
        ADD_TEST(TestSuccessed),
        ADD_TEST(TestFailed),
    )
}

namespace GroupSuccessedComposite {
    namespace InnerGroup {
        TEST_GROUP("Inner Successed Group",
            ADD_TEST(TestSuccessed),
            ADD_TEST(TestSuccessed),
        )
    }
    TEST_GROUP("Composite Successed Group",
        ADD_TEST(TestSuccessed),
        ADD_TEST(TestSuccessed),
        ADD_GROUP(InnerGroup),
    )
}

namespace GroupFailedComposite {
    namespace InnerGroup {
        TEST_GROUP("Inner Failed Group",
            ADD_TEST(TestSuccessed),
            ADD_TEST(TestFailed),
        )
    }
    TEST_GROUP("Composite Failed Group",
        ADD_TEST(TestSuccessed),
        ADD_TEST(TestSuccessed),
        ADD_GROUP(InnerGroup),
    )
}

namespace GroupOneSuccessed {
    TEST_GROUP("Group with one successed item",
        ADD_TEST(TestSuccessed)
    )
}

namespace GroupOneFailed {
    TEST_GROUP("Group with one failed item",
        ADD_TEST(TestFailed)
    )
}

namespace GroupRun {
    TEST_GROUP("Standalone Group Run",
        ADD_TEST(RunSuccessfulGroup),
        ADD_TEST(RunFailureGroup),
        ADD_TEST(RunSuccessfulCompoundGroup),
        ADD_TEST(RunFailureCompoundGroup),
    )
}

namespace ConditionAssertion {
    TEST_GROUP("Condition Assertion",
        ADD_TEST(TestAssertIsTruePassed),
        ADD_TEST(TestAssertIsTrueFailed),
        ADD_TEST(TestAssertIsFalsePassed),
        ADD_TEST(TestAssertIsFalseFailed),
    )
}

TEST_SUITE(
    ADD_TEST(TestSuccessed),
    ADD_TEST(TestFailed),
    ADD_CLASS(TestClassSuccessed),
    ADD_CLASS(TestClassFailed),
    ADD_GROUP(GroupSuccessed),
    ADD_GROUP(GroupSuccessedComposite),
    ADD_GROUP(GroupFailedComposite),
    ADD_GROUP(GroupOneSuccessed),
    ADD_GROUP(GroupOneFailed),
    ADD_GROUP(GroupRun),
    ADD_GROUP(ConditionAssertion),
)

} // namespace TestWindowTests
