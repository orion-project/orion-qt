#include "../testing/OriTestBase.h"
#include "../core/OriVersion.h"

namespace Ori {
namespace Tests {
namespace VersionTests {

//------------------------------------------------------------------------------

TEST_METHOD(constructor_int)
{
    Version v0;
    TEST_LOG(v0.str(3))
    ASSERT_IS_TRUE(v0.match(0, 0, 0))

    Version v1(2);
    TEST_LOG(v1.str(3))
    ASSERT_IS_TRUE(v1.match(2, 0, 0))

    Version v2(3, 4);
    TEST_LOG(v2.str(3))
    ASSERT_IS_TRUE(v2.match(3, 4, 0))

    Version v3(5, 6, 7);
    TEST_LOG(v3.str(3))
    ASSERT_IS_TRUE(v3.match(5, 6, 7))
}

TEST_METHOD(constructor_str)
{
    Version v1("2");
    TEST_LOG(v1.str(3))
    ASSERT_IS_TRUE(v1.match(2, 0, 0))

    Version v2("3.4");
    TEST_LOG(v2.str(3))
    ASSERT_IS_TRUE(v2.match(3, 4, 0))

    Version v3("5.6.7");
    TEST_LOG(v3.str())
    ASSERT_IS_TRUE(v3.match(5, 6, 7))

    Version v4("8.9.2.3");
    TEST_LOG(v4.str())
    ASSERT_IS_TRUE(v4.match(8, 9, 2))
    
    Version v5("2.0.1-beta2");
    TEST_LOG(v5.str())
    ASSERT_IS_TRUE(v5.match(2, 0, 1))

    Version v6("2.0.1-beta-2");
    TEST_LOG(v6.str())
    ASSERT_IS_TRUE(v6.match(2, 0, 1))
}

TEST_METHOD(constructor_str_invalid)
{
    Version v0("this not a version");
    ASSERT_IS_TRUE(v0.match(0))

    Version v1("this not a version.1.2");
    ASSERT_IS_TRUE(v1.match(0))

    Version v2("1.this not a version.2");
    ASSERT_IS_TRUE(v2.match(0))

    Version v3("1.2.this not a version");
    ASSERT_IS_TRUE(v3.match(0))

    Version v4("1.2.3beta");
    ASSERT_IS_TRUE(v3.match(0))
}

TEST_METHOD(more_and_less)
{
    Version v2(1, 5, 5);
    ASSERT_IS_TRUE(v2.less(2, 1, 1))
    ASSERT_IS_TRUE(v2.less(1, 6, 1))
    ASSERT_IS_TRUE(v2.less(1, 5, 7))
    ASSERT_IS_TRUE(v2.more(0, 6, 7))
    ASSERT_IS_TRUE(v2.more(1, 4, 7))
    ASSERT_IS_TRUE(v2.more(1, 5, 4))
}

TEST_METHOD(operators)
{
    Version v1(1, 0);
    Version v2(1, 0);
    Version v3(1, 1);
    ASSERT_IS_TRUE(v1 == v2)
    ASSERT_IS_TRUE(v1 <= v2)
    ASSERT_IS_TRUE(v1 >= v2)
    ASSERT_IS_TRUE(v1 != v3)
    ASSERT_IS_TRUE(v1 < v3)
    ASSERT_IS_TRUE(v1 <= v3)
    ASSERT_IS_TRUE(v3 > v1)
    ASSERT_IS_TRUE(v3 >= v1)
}

TEST_METHOD(isValid)
{
    Version v0;
    ASSERT_IS_FALSE(v0.isValid());

    Version v1(1);
    ASSERT_IS_TRUE(v1.isValid());

    Version v2(0, 1);
    ASSERT_IS_TRUE(v2.isValid());

    Version v3(0, 0, 1);
    ASSERT_IS_TRUE(v3.isValid());
}

TEST_METHOD(str)
{
    Version v("5.6.7");
    ASSERT_EQ_STR(v.str(-1), "5")
    ASSERT_EQ_STR(v.str(0), "5")
    ASSERT_EQ_STR(v.str(1), "5")
    ASSERT_EQ_STR(v.str(), "5.6")
    ASSERT_EQ_STR(v.str(2), "5.6")
    ASSERT_EQ_STR(v.str(3), "5.6.7")
    ASSERT_EQ_STR(v.str(5), "5.6.7")
}

//------------------------------------------------------------------------------

TEST_GROUP("Version",
    ADD_TEST(constructor_int),
    ADD_TEST(constructor_str),
    ADD_TEST(constructor_str_invalid),
    ADD_TEST(more_and_less),
    ADD_TEST(operators),
    ADD_TEST(isValid),
    ADD_TEST(str)
)

} // namespace VersionTests
} // namespace Tests
} // namespace Ori
