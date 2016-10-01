#include "../testing/OriTestBase.h"
#include "../core/OriVersion.h"

namespace Ori {
namespace Test {
namespace VersionTests {

////////////////////////////////////////////////////////////////////////////////

TEST_METHOD(constructors)
{
    Version v0;
    ASSERT_IS_TRUE(v0.match(0, 0))
    TEST_LOG(v0.str())

    Version v1(3, 4);
    ASSERT_IS_TRUE(v1.match(3, 4))
    TEST_LOG(v1.str())

    Version v2("5.6.7");
    ASSERT_IS_TRUE(v2.match(5, 6))
    TEST_LOG(v2.str())
}

TEST_METHOD(more_and_less)
{
    Version v1(1, 5);
    ASSERT_IS_TRUE(v1.less(2, 0))
    ASSERT_IS_TRUE(v1.less(1, 6))
    ASSERT_IS_TRUE(v1.more(1, 1))
    ASSERT_IS_TRUE(v1.more(0, 4))
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

////////////////////////////////////////////////////////////////////////////////

TEST_GROUP("Version",
    ADD_TEST(constructors),
    ADD_TEST(more_and_less),
    ADD_TEST(operators)
)

} // namespace VersionTests
} // namespace Test
} // namespace Ori
