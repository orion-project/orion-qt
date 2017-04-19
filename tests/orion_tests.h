#ifndef ORI_TESTS_H
#define ORI_TESTS_H

#include "../testing/OriTestBase.h"

namespace Ori {
namespace Test {

USE_GROUP(MathTests)        // ori_test_math.cpp
USE_GROUP(TemplatesTests)   // ori_test_Templates.cpp
USE_GROUP(VersionTests)     // ori_test_Version.cpp
USE_GROUP(FilterTests)      // ori_test_Filter.cpp

TEST_SUITE(
    ADD_GROUP(MathTests),
    ADD_GROUP(TemplatesTests),
    ADD_GROUP(VersionTests),
    ADD_GROUP(FilterTests),
)

namespace All {
    TEST_GROUP("Orion",
        ADD_GROUP(MathTests),
        ADD_GROUP(TemplatesTests),
        ADD_GROUP(VersionTests),
        ADD_GROUP(FilterTests),
    )
}

} // namespace Test
} // namespace Ori

#endif // ORI_TESTS_H
