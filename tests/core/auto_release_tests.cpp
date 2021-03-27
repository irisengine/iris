#include <gtest/gtest.h>

#include "core/auto_release.h"

namespace
{

using AutoIntPtr = iris::AutoRelease<int *, nullptr>;

void deleter(int *value)
{
    --*value;
}

}

TEST(auto_release, ctor)
{
    int x = 1;
    AutoIntPtr v{&x, deleter};

    ASSERT_EQ(v.get(), &x);
    ASSERT_EQ(static_cast<int *>(v), &x);
    ASSERT_TRUE(v);
}

TEST(auto_release, function_deleter)
{
    int x = 1;

    {
        AutoIntPtr v{&x, deleter};
    }

    ASSERT_EQ(x, 0);
}

TEST(auto_release, lambda_deleter)
{
    int x = 1;
    int y = 2;

    {
        AutoIntPtr v{&x, [y](int *p) { *p += y; }};
    }

    ASSERT_EQ(x, 3);
}

TEST(auto_release, invalid)
{
    AutoIntPtr v{nullptr, deleter};

    ASSERT_EQ(v.get(), nullptr);
    ASSERT_FALSE(v);
}

TEST(auto_release, move_ctor)
{
    int x = 1;

    {
        AutoIntPtr v1{&x, deleter};
        AutoIntPtr v2{std::move(v1)};

        ASSERT_FALSE(v1);
        ASSERT_EQ(v1.get(), nullptr);
        ASSERT_TRUE(v2);
        ASSERT_EQ(v2.get(), &x);
    }

    ASSERT_EQ(x, 0);
}

TEST(auto_release, move_assignment)
{
    int x = 1;
    int y = 1;

    {
        AutoIntPtr v1{&x, deleter};
        AutoIntPtr v2{&y, deleter};

        v2 = std::move(v1);

        ASSERT_FALSE(v1);
        ASSERT_EQ(v1.get(), nullptr);
        ASSERT_TRUE(v2);
        ASSERT_EQ(v2.get(), &x);
    }

    ASSERT_EQ(x, 0);
    ASSERT_EQ(y, 0);
}

TEST(auto_release, address)
{
    int x = 1;

    {
        AutoIntPtr v{nullptr, deleter};

        const auto setter = [&x](int **p) { *p = std::addressof(x); };
        setter(&v);
    }

    ASSERT_EQ(x, 0);
}
