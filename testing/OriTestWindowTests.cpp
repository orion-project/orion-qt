#include "OriTestBase.h"

#include <QMutex>
#include <QVector>
#include <QThread>

using namespace Ori::Testing;

/// Tests for testing the test window
namespace TestWindowTests {

TEST_METHOD(TestSuccessed)
{
    QThread::msleep(100);
    ASSERT_OK("OK")
}

TEST_METHOD(TestFailed)
{
    QThread::msleep(100);
    ASSERT_IS_TRUE(false);
}

TEST_METHOD(TestAssertIsTruePassed)
{
    QThread::msleep(100);
    ASSERT_IS_TRUE(2+2 == 4)
}

TEST_METHOD(TestAssertIsTrueFailed)
{
    QThread::msleep(100);
    ASSERT_IS_TRUE(2+2 == 5)
}

TEST_METHOD(TestAssertIsFalsePassed)
{
    QThread::msleep(100);
    ASSERT_IS_FALSE(2 == 3)
}

TEST_METHOD(TestAssertIsFalseFailed)
{
    QThread::msleep(100);
    ASSERT_IS_FALSE(2 != 3)
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

namespace ConditionAssertion {
    TEST_GROUP("Condition Assertion",
        ADD_TEST(TestAssertIsTruePassed),
        ADD_TEST(TestAssertIsTrueFailed),
        ADD_TEST(TestAssertIsFalsePassed),
        ADD_TEST(TestAssertIsFalseFailed),
    )
}

namespace {
TEST_CLASS(TestClassSuccessed)
    void run() override
    {
        TestBase::run();
        TestBase *test = this;
        ASSERT_IS_TRUE(2 == 2);
    }
TEST_CLASS_END

TEST_CLASS(TestClassFailed)
    void run() override
    {
        TestBase::run();
        TestBase *test = this;
        ASSERT_IS_TRUE(2 == 3);
    }
TEST_CLASS_END
}

namespace BeforeAndAfter {
    namespace GroupWithBeforeAndAfterAll {
        TEST_METHOD(before_all) {
            TEST_LOG("before all")
        }

        TEST_METHOD(after_all) {
            TEST_LOG("after all")
        }

        TEST_GROUP("Group with before and after",
            BEFORE_ALL(before_all),
            ADD_TEST(TestSuccessed),
            ADD_TEST(TestSuccessed),
            ADD_TEST(TestSuccessed),
            AFTER_ALL(after_all),
        )
    }

    namespace GroupWithBeforeAllFailed {
        TEST_METHOD(before_all) {
            ASSERT_FAIL("Failed BEFORE_ALL must stop its group")
        }

        TEST_GROUP("Group with failed BEFORE_ALL",
            BEFORE_ALL(before_all),
            ADD_TEST(TestSuccessed),
            ADD_TEST(TestSuccessed),
            ADD_TEST(TestSuccessed),
        )
    }

    namespace PassedGroupWithAfterAllFailed {
        TEST_METHOD(after_all) {
            TEST_LOG("Failed AFTER_ALL must fail its group")
            ASSERT_IS_TRUE(false)
        }

        TEST_GROUP("Passed group with failed AFTER_ALL",
            ADD_TEST(TestSuccessed),
            ADD_TEST(TestSuccessed),
            ADD_TEST(TestSuccessed),
            AFTER_ALL(after_all),
        )
    }

    namespace FailedGroupWithAfterAllFailed {
        TEST_METHOD(after_all) {
            TEST_LOG("Failed AFTER_ALL must not override fail message of its group")
            ASSERT_IS_TRUE(false)
        }

        TEST_GROUP("Failed group with failed AFTER_ALL",
            ADD_TEST(TestSuccessed),
            ADD_TEST(TestFailed),
            ADD_TEST(TestSuccessed),
            AFTER_ALL(after_all),
        )
    }

    TEST_GROUP("Before/After methods",
        ADD_GROUP(GroupWithBeforeAndAfterAll),
        ADD_GROUP(GroupWithBeforeAllFailed),
        ADD_GROUP(PassedGroupWithAfterAllFailed),
        ADD_GROUP(FailedGroupWithAfterAllFailed),
    )
} // namespace BeforeAndAfter


TEST_SUITE(
    ADD_TEST(TestSuccessed),
    ADD_TEST(TestFailed),
    ADD_GROUP(GroupOneSuccessed),
    ADD_GROUP(GroupOneFailed),
    ADD_GROUP(GroupSuccessed),
    ADD_GROUP(GroupFailed),
    ADD_GROUP(GroupSuccessedComposite),
    ADD_GROUP(GroupFailedComposite),
    ADD_GROUP(ConditionAssertion),
    ADD_CLASS(TestClassSuccessed),
    ADD_CLASS(TestClassFailed),
    ADD_GROUP(BeforeAndAfter),
)

} // namespace TestWindowTests
