#include "testing/OriTestBase.h"

#include <QtMath>

// This is cpp-file containing test group.

// Put your test group in a separate namespace:
namespace SampleTestGroup {

// Define test methods.
TEST_METHOD(test_method_1)
{
    int a = 1;
    ASSERT_EQ_INT(a, 2);
}

TEST_METHOD(test_method_2)
{
    double a = 3.14;
    ASSERT_EQ_DBL(a, M_PI)
}

// Define group and populate it with methods.
TEST_GROUP("Test group example",
    ADD_TEST(test_method_1),
    ADD_TEST(test_method_2),
)

} // namespace SampleTestGroup
