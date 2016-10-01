#ifndef ORI_TESTS_H
#define ORI_TESTS_H

#include "../testing/OriTestBase.h"

namespace Ori {
namespace Test {

USE_GROUP(MathTests)        // test_math.cpp
USE_GROUP(TemplatesTests)   // test_Templates.cpp
USE_GROUP(VersionTests)     // test_Version.cpp

TEST_SUITE(
    ADD_GROUP(MathTests),
    ADD_GROUP(TemplatesTests),
    ADD_GROUP(VersionTests)
)

} // namespace Test
} // namespace Ori

#endif // ORI_TESTS_H
