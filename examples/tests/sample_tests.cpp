#include "testing/OriTestBase.h"

// Test group can be defined in a separate cpp-file (see sample_testgroup.cpp).
// This file must be attached to pro-file to be compiled,
// and group must be referenced before be added to suite:
USE_GROUP(SampleTestGroup)

// Define your test suite as a separate namespace:
namespace SampleTestsSuite {

// Or group can be defined just in test suite
namespace AnotherTestGroup {

    // Define test methods.
    TEST_METHOD(test_method_1)
    {
        void *ptr = nullptr;
        ASSERT_IS_NOT_NULL(ptr)
    }

   TEST_GROUP("Second test group",
      ADD_TEST(test_method_1))
}


// Or test can be defined whithout a group
TEST_METHOD(top_level_test_method)
{
    ASSERT_FAIL("Something wrong")
}


// Populate suite with tests
TEST_SUITE(
    ADD_TEST(top_level_test_method),
    ADD_GROUP(SampleTestGroup),
    ADD_GROUP(AnotherTestGroup),
)

} // namespace SampleTestsSuite
