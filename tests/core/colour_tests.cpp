////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "core/colour.h"

TEST(colour, empty_ctor)
{
    const iris::Colour c{};

    ASSERT_EQ(c.r, 0.0f);
    ASSERT_EQ(c.g, 0.0f);
    ASSERT_EQ(c.b, 0.0f);
    ASSERT_EQ(c.a, 1.0f);
}

TEST(colour, float_ctor)
{
    const iris::Colour c{0.1f, 0.2f, 0.3f, 0.4f};

    ASSERT_EQ(c.r, 0.1f);
    ASSERT_EQ(c.g, 0.2f);
    ASSERT_EQ(c.b, 0.3f);
    ASSERT_EQ(c.a, 0.4f);
}

TEST(colour, byte_ctor)
{
    const iris::Colour c{0x0, 0x7f, 0xff, 0x33};

    ASSERT_EQ(c, iris::Colour(0.0f, 0.498039216f, 1.0f, 0.2f));
}

TEST(colour, byte_ctor_no_alpha)
{
    const iris::Colour c{0x0, 0x7f, 0xff};

    ASSERT_EQ(c, iris::Colour(0.0f, 0.498039216f, 1.0f, 1.0f));
}

TEST(colour, scale)
{
    iris::Colour c1{1.1f, 2.2f, 3.3f};
    auto c2 = c1 * 2.0f;

    ASSERT_EQ(c2, iris::Colour(2.2f, 4.4f, 6.6f, 2.0f));
}

TEST(colour, scale_assignment)
{
    iris::Colour c{1.1f, 2.2f, 3.3f};
    c *= 2.0f;

    ASSERT_EQ(c, iris::Colour(2.2f, 4.4f, 6.6f, 2.0f));
}

TEST(colour, add)
{
    iris::Colour c1{1.1f, 2.2f, 3.3f};
    auto c2 = c1 + iris::Colour{1.0f, 2.0f, 3.0f};

    ASSERT_EQ(c2, iris::Colour(2.1f, 4.2f, 6.3f, 2.0f));
}

TEST(colour, add_assignment)
{
    iris::Colour c{1.1f, 2.2f, 3.3f};
    c += iris::Colour{1.0f, 2.0f, 3.0f};

    ASSERT_EQ(c, iris::Colour(2.1f, 4.2f, 6.3f, 2.0f));
}

TEST(colour, subtract)
{
    iris::Colour c1{1.1f, 2.2f, 3.3f};
    auto c2 = c1 - iris::Colour{1.0f, 2.0f, 3.0f};

    ASSERT_EQ(c2, iris::Colour(0.1f, 0.2f, 0.3f, 0.0f));
}

TEST(colour, subtract_assignment)
{
    iris::Colour c{1.1f, 2.2f, 3.3f};
    c -= iris::Colour{1.0f, 2.0f, 3.0f};

    ASSERT_EQ(c, iris::Colour(0.1f, 0.2f, 0.3f, 0.0f));
}

TEST(colour, multiply)
{
    iris::Colour c1{1.1f, 2.2f, 3.3f};
    auto c2 = c1 * iris::Colour{1.0f, 2.0f, 3.0f};

    ASSERT_EQ(c2, iris::Colour(1.1f, 4.4f, 9.9f, 1.0f));
}

TEST(colour, multiply_assignment)
{
    iris::Colour c{1.1f, 2.2f, 3.3f};
    c *= iris::Colour{1.0f, 2.0f, 3.0f};

    ASSERT_EQ(c, iris::Colour(1.1f, 4.4f, 9.9f, 1.0f));
}

TEST(colour, equality)
{
    const iris::Colour c1{0.1f, 0.2f, 0.3f, 0.4f};
    const auto c2 = c1;

    ASSERT_EQ(c1, c2);
}

TEST(colour, inequality)
{
    const iris::Colour c1{0.1f, 0.2f, 0.3f, 0.4f};
    const iris::Colour c2{0.1f, 0.2f, 0.4f, 0.4f};

    ASSERT_NE(c1, c2);
}
