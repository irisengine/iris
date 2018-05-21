#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

#include "cactus_stack.hpp"

TEST(cactus_stack_tests, copy_constructor)
{
    eng::cactus_stack<std::uint32_t> stack1{ };
    stack1.emplace(0u);
    stack1.emplace(1u);
    stack1.pop();
    stack1.emplace(2u);
    stack1.emplace(3u);
    stack1.pop();
    stack1.emplace(4u);

    eng::cactus_stack<std::uint32_t> stack2(stack1);

    auto iter1 = std::begin(stack1);
    auto iter2 = std::begin(stack2);

    while(iter1 != std::end(stack1))
    {
        ASSERT_EQ(*iter1, *iter2);
        ++iter1;
        ++iter2;
    }

    ASSERT_EQ(std::end(stack2), iter2);
}

TEST(cactus_stack_tests, copy_assignment)
{
    eng::cactus_stack<std::uint32_t> stack1{ };
    stack1.emplace(0u);
    stack1.emplace(1u);
    stack1.pop();
    stack1.emplace(2u);
    stack1.emplace(3u);
    stack1.pop();
    stack1.emplace(4u);

    eng::cactus_stack<std::uint32_t> stack2{ };
    stack2 = stack1;

    auto iter1 = std::begin(stack1);
    auto iter2 = std::begin(stack2);

    while(iter1 != std::end(stack1))
    {
        ASSERT_EQ(*iter1, *iter2);
        ++iter1;
        ++iter2;
    }

    ASSERT_EQ(std::end(stack2), iter2);
}

TEST(cactus_stack_tests, push)
{
    eng::cactus_stack<std::uint32_t> stack{ };

    stack.push(3u);
    ASSERT_EQ(3u, stack.top());

    stack.push(4u);
    ASSERT_EQ(4u, stack.top());

    stack.push(5u);
    ASSERT_EQ(5u, stack.top());
}

TEST(cactus_stack_tests, pop)
{
    eng::cactus_stack<std::uint32_t> stack{ };

    stack.push(3u);
    stack.push(4u);
    stack.push(5u);

    ASSERT_EQ(5u, stack.top());
    stack.pop();
    ASSERT_EQ(4u, stack.top());
    stack.pop();
    ASSERT_EQ(3u, stack.top());
}

TEST(cactus_stack, emplace)
{
    struct A
    {
        A(std::uint32_t x, float y)
            : x(x),
              y(y)
        { }

        std::uint32_t x;
        float y;
    };

    eng::cactus_stack<A> stack{ };

    stack.emplace(3u, 1.5f);

    ASSERT_EQ(3u, stack.top().x);
    ASSERT_EQ(1.5f, stack.top().y);
}

TEST(cactus_stack, basic_iterator)
{
    eng::cactus_stack<std::uint32_t> stack{ };

    ASSERT_EQ(stack.begin(), stack.end());
    ASSERT_EQ(stack.cbegin(), stack.cend());
}

TEST(cactus_stack, iterator_deref)
{
    eng::cactus_stack<std::uint32_t> stack{ };
    stack.emplace(3u);

    ASSERT_EQ(3u, *stack.begin());
}

TEST(cactus_stack, iterator_equals)
{
    eng::cactus_stack<std::uint32_t> stack{ };
    stack.emplace(3u);

    ASSERT_EQ(stack.begin(), stack.begin());
}

TEST(cactus_stack, iterator_not_equals)
{
    eng::cactus_stack<std::uint32_t> stack{ };
    stack.emplace(3u);
    stack.emplace(4u);

    ASSERT_NE(stack.begin(), stack.begin() + 1);
}

TEST(cactus_stack, iterator_pre_increment)
{
    eng::cactus_stack<std::uint32_t> stack{ };
    stack.emplace(3u);
    stack.emplace(4u);

    auto iter = std::begin(stack);
    ASSERT_EQ(3u, *++iter);
    ASSERT_EQ(3u, *iter);
}

TEST(cactus_stack, iterator_post_increment)
{
    eng::cactus_stack<std::uint32_t> stack{ };
    stack.emplace(3u);
    stack.emplace(4u);

    auto iter = std::begin(stack);
    ASSERT_EQ(4u, *iter++);
    ASSERT_EQ(3u, *iter++);
    ASSERT_EQ(std::end(stack), iter);
}

TEST(cactus_stack, iterator_add)
{
    eng::cactus_stack<std::uint32_t> stack{ };
    stack.emplace(3u);
    stack.emplace(4u);
    stack.emplace(5u);

    auto iter = std::begin(stack) + 2u;
    ASSERT_EQ(3u, *iter);
}

TEST(cactus_stack, iterator_add_equls)
{
    eng::cactus_stack<std::uint32_t> stack{ };
    stack.emplace(3u);
    stack.emplace(4u);
    stack.emplace(5u);

    auto iter = std::begin(stack);
    iter += 2u;
    ASSERT_EQ(3u, *iter);
}

TEST(cactus_stack, clear)
{
    eng::cactus_stack<std::uint32_t> stack{ };
    stack.emplace(0u);

    ASSERT_NE(stack.cbegin(), stack.cend());

    stack.clear();
    ASSERT_EQ(stack.cbegin(), stack.cend());
}

TEST(cactus_stack, iterate)
{
    eng::cactus_stack<std::uint32_t> stack{ };

    for(auto i = 0u; i < 5u; ++i)
    {
        stack.emplace(i);
    }

    auto counter = 4u;
    for(const auto item : stack)
    {
        ASSERT_EQ(counter, item);
        --counter;
    }
}

TEST(cactus_stack, iterate_complex)
{
    eng::cactus_stack<std::uint32_t> stack{ };

    stack.emplace(0u);
    stack.emplace(1u);
    stack.pop();
    stack.emplace(2u);
    stack.emplace(3u);
    stack.pop();
    stack.emplace(4u);

    std::vector<std::uint32_t> expected { 4u, 2u, 0u };

    auto expected_iter = std::begin(expected);
    auto actual_iter = std::begin(stack);

    while(expected_iter != std::end(expected))
    {
        ASSERT_EQ(*expected_iter, *actual_iter);
        ++expected_iter;
        ++actual_iter;
    }

    ASSERT_EQ(std::end(stack), actual_iter);
}

