#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "core/real.h"

TEST(real, constructor)
{
    iris::real r{ 1.1f };
    ASSERT_EQ(r.value, 1.1f);
}

TEST(real, implicit_constructor)
{
    iris::real r = 1.1f;
    ASSERT_EQ(r.value, 1.1f);
}

TEST(real, cast)
{
    iris::real r = 1.1f;
    ASSERT_EQ(static_cast<float>(r), 1.1f);
}

TEST(real, const_cast)
{
    const iris::real r = 1.1f;
    ASSERT_EQ(static_cast<float>(r), 1.1f);
}

TEST(real, equality)
{
    iris::real r1 = 0.1f;
    iris::real r2 = 0.1f;

    // create 1.0 with two different methods (addition and multiplication)
    // these are known to produce two different values due to float limitations

    for(int i = 0; i < 9; ++i)
    {
        r1 += 0.1f;
    }

    r2 *= 10;

    // check that our values are different bit for bit, but the real class
    // equality handles this
    ASSERT_NE(r1.value, r2.value);
    ASSERT_EQ(r1, r2);
}

TEST(real, inequality)
{
    iris::real r1 = 0.1f;
    iris::real r2 = 0.11f;

    ASSERT_NE(r1, r2);
}

TEST(real, less)
{
    ASSERT_LT(iris::real{ 1.0f }, iris::real{ 2.0f });
    ASSERT_FALSE(iris::real{ 1.0f } < iris::real{ 1.0f });
}

TEST(real, less_cast)
{
    ASSERT_LT(iris::real{ 1.0f }, 2.0);
    ASSERT_FALSE(iris::real{ 1.0f } < 1.0);
}

TEST(real, less_equal)
{
    ASSERT_LE(iris::real{ 1.0f }, iris::real{ 2.0f });
    ASSERT_LE(iris::real{ 1.0f }, iris::real{ 1.0f });
}

TEST(real, less_equal_cast)
{
    ASSERT_LE(iris::real{ 1.0f }, 2.0);
    ASSERT_LE(iris::real{ 1.0f }, 1.0);
}

TEST(real, greater)
{
    ASSERT_GT(iris::real{ 3.0f }, iris::real{ 2.0f });
    ASSERT_FALSE(iris::real{ 1.0f } > iris::real{ 1.0f });
}

TEST(real, greater_cast)
{
    ASSERT_GT(iris::real{ 3.0f }, 2.0);
    ASSERT_FALSE(iris::real{ 1.0f } > 1.0);
}

TEST(real, greater_equal)
{
    ASSERT_GE(iris::real{ 3.0f }, iris::real{ 2.0f });
    ASSERT_GE(iris::real{ 1.0f }, iris::real{ 1.0f });
}

TEST(real, greater_equal_cast)
{
    ASSERT_GE(iris::real{ 3.0f }, 2.0);
    ASSERT_GE(iris::real{ 1.0f }, 1.0);
}

TEST(real, ostream)
{
    std::stringstream strm1;
    std::stringstream strm2;

    iris::real v1 = 1.234f;
    float v2 = 1.234f;

    strm1 << v1;
    strm2 << v2;

    ASSERT_EQ(strm1.str(), strm2.str());
}

TEST(real, istream)
{
    std::string str = "1.234";
    std::stringstream strm;
    strm << str;

    iris::real r = 0.0f;
    strm >> r;

    ASSERT_EQ(r.value, 1.234f);
}

