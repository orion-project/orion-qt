#include "../testing/OriTestBase.h"
#include "../core/OriFloatingPoint.h"

#include <cmath>

#ifdef Q_OS_MACOS
#define __isnan std::isnan
#endif

namespace Ori {
namespace Tests {
namespace MathTests {

TEST_METHOD(learn_nan)
{
    double nan = Double::nan();
    ASSERT_IS_TRUE(Double(nan).isNan())
    ASSERT_IS_TRUE(std::isnan(nan));
    ASSERT_IS_TRUE(__isnan(nan));
    ASSERT_IS_FALSE(nan > 0);
    ASSERT_IS_FALSE(nan < 0);
    ASSERT_IS_FALSE(2 > nan);
    ASSERT_IS_FALSE(2 < nan);
    ASSERT_IS_FALSE(nan == 0);
    ASSERT_IS_FALSE(2 == nan);
    ASSERT_IS_FALSE(nan == nan);

    ASSERT_IS_TRUE(__isnan(2 * nan));
    ASSERT_IS_TRUE(__isnan(nan * 2));
    ASSERT_IS_TRUE(__isnan(2 / nan));
    ASSERT_IS_TRUE(__isnan(nan / 2.0));
    ASSERT_IS_TRUE(__isnan(2 + nan));
    ASSERT_IS_TRUE(__isnan(nan + 2));
}

TEST_METHOD(learn_infinity)
{
    double inf = Double::infinity();
    ASSERT_IS_TRUE(Double(inf).isInfinity());
    ASSERT_IS_TRUE(std::isinf(inf));
    ASSERT_IS_TRUE(inf == std::numeric_limits<double>::infinity());
    ASSERT_IS_TRUE(inf > 0);
    ASSERT_IS_FALSE(inf < 0);
    ASSERT_IS_FALSE(inf == 0);

    double pos_inf = +Double::infinity();
    ASSERT_IS_TRUE(Double(pos_inf).isInfinity());
    ASSERT_IS_TRUE(std::isinf(pos_inf));
    ASSERT_IS_TRUE(pos_inf == +std::numeric_limits<double>::infinity());
    ASSERT_IS_TRUE(pos_inf == +1/0.0);
    ASSERT_IS_TRUE(pos_inf > 0);
    ASSERT_IS_FALSE(pos_inf < 0);
    ASSERT_IS_FALSE(pos_inf == 0);

    double neg_inf = -Double::infinity();
    ASSERT_IS_TRUE(Double(neg_inf).isInfinity());
    ASSERT_IS_TRUE(std::isinf(neg_inf));
    ASSERT_IS_TRUE(neg_inf == -std::numeric_limits<double>::infinity());
    ASSERT_IS_TRUE(neg_inf == -1/0.0);
    ASSERT_IS_FALSE(neg_inf > 0);
    ASSERT_IS_TRUE(neg_inf < 0);
    ASSERT_IS_FALSE(neg_inf == 0);

    ASSERT_IS_TRUE(__isnan(pos_inf + neg_inf));

    ASSERT_IS_TRUE(std::isnan(inf * 0.0));
    ASSERT_IS_TRUE(std::isnan(0.0 * inf));
    ASSERT_IS_TRUE(std::isinf(inf / 0.0));
    ASSERT_IS_TRUE(0.0 / inf == 0);

    ASSERT_IS_TRUE(std::isinf(inf * 2.0));
    ASSERT_IS_TRUE(std::isinf(2.0 * inf));
    ASSERT_IS_TRUE(std::isinf(inf / 2.0));
    ASSERT_IS_TRUE(2 / inf == 0.0);
    ASSERT_IS_TRUE(std::isinf(inf + 2.0));
    ASSERT_IS_TRUE(std::isinf(2.0 + inf));
}

// The IEEE 754 floating-point standard specifies that every floating point
// arithmetic operation, including division by zero, has a well-defined result.
// The standard supports signed zero, as well as infinity and NaN (not a number).
TEST_METHOD(division_by_zero)
{
    double pos_inf = 1 / 0.0;
    TEST_LOG_DOUBLE(pos_inf);
    ASSERT_IS_TRUE(Double(pos_inf).isInfinity());
    ASSERT_IS_TRUE(std::isinf(pos_inf));
    ASSERT_IS_TRUE(pos_inf > 0);

    double neg_inf = -1 / 0.0;
    TEST_LOG_DOUBLE(neg_inf);
    ASSERT_IS_TRUE(Double(neg_inf).isInfinity());
    ASSERT_IS_TRUE(std::isinf(neg_inf));
    ASSERT_IS_TRUE(neg_inf < 0);

    double nan = 0 / 0.0;
    TEST_LOG_DOUBLE(nan);
    ASSERT_IS_TRUE(__isnan(nan));

    ASSERT_IS_TRUE(1/+0.0 == pos_inf);
    ASSERT_IS_TRUE(1/-0.0 == neg_inf);
}

TEST_METHOD(invalid_sqrt)
{
    ASSERT_IS_TRUE(__isnan(std::sqrt(nan(""))));
    ASSERT_IS_TRUE(__isnan(sqrt(nan(""))));

    ASSERT_IS_TRUE(sqrt(0) == 0);

    ASSERT_IS_TRUE(__isnan(std::sqrt(-1)));
    ASSERT_IS_TRUE(__isnan(sqrt(-1)));

    ASSERT_IS_TRUE(std::isinf(std::sqrt(Double::infinity())));
    ASSERT_IS_TRUE(std::isinf(sqrt(Double::infinity())));

    ASSERT_IS_TRUE(std::isinf(std::sqrt(1/0.0)));
    ASSERT_IS_TRUE(std::isinf(sqrt(1/0.0)));
    ASSERT_IS_TRUE(std::isnan(std::sqrt(-1/0.0)));
    ASSERT_IS_TRUE(std::isnan(sqrt(-1/0.0)));
}

//------------------------------------------------------------------------------

TEST_GROUP("Math",
    ADD_TEST(learn_nan),
    ADD_TEST(learn_infinity),
    ADD_TEST(division_by_zero),
    ADD_TEST(invalid_sqrt)
)

} // namespace MathTests
} // namespace Tests
} // namespace Ori
