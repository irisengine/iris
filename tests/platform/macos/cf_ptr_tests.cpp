#include <gtest/gtest.h>

#include <CoreFoundation/CoreFoundation.h>

#include "platform/macos/cf_ptr.h"

TEST(cf_ptr_tests, get)
{
    auto cf_string = ::CFStringCreateWithCString(nullptr, "hello", kCFStringEncodingUTF8);

    const eng::CfPtr ptr{ cf_string };

    ASSERT_EQ(kCFCompareEqualTo, ::CFStringCompare(ptr.get(), CFSTR("hello"), 0));
}

TEST(cf_ptr_tests, bool_operator)
{
    eng::CfPtr<CFStringRef> ptr1{ nullptr };
    ASSERT_FALSE(ptr1);

    eng::CfPtr<CFStringRef> ptr2{ ::CFStringCreateWithCString(nullptr, "hello", kCFStringEncodingUTF8) };
    ASSERT_TRUE(ptr2);
}

TEST(cf_ptr_tests, swap)
{
    auto cf_string1 = ::CFStringCreateWithCString(nullptr, "hello", kCFStringEncodingUTF8);
    auto cf_string2 = ::CFStringCreateWithCString(nullptr, "world", kCFStringEncodingUTF8);

    eng::CfPtr ptr1(cf_string1);
    eng::CfPtr ptr2(cf_string2);
    ptr1.swap(ptr2);

    ASSERT_EQ(kCFCompareEqualTo, ::CFStringCompare(ptr1.get(), CFSTR("world"), 0));
    ASSERT_EQ(kCFCompareEqualTo, ::CFStringCompare(ptr2.get(), CFSTR("hello"), 0));
}

TEST(cf_ptr_tests, move_constructor)
{
    auto cf_string1 = ::CFStringCreateWithCString(nullptr, "hello", kCFStringEncodingUTF8);

    eng::CfPtr ptr1(cf_string1);
    eng::CfPtr ptr2{ std::move(ptr1) };

    ASSERT_FALSE(ptr1);
    ASSERT_EQ(kCFCompareEqualTo, ::CFStringCompare(ptr2.get(), CFSTR("hello"), 0));
}

TEST(cf_ptr_tests, move_assignment)
{
    auto cf_string1 = ::CFStringCreateWithCString(nullptr, "hello", kCFStringEncodingUTF8);

    eng::CfPtr ptr1(cf_string1);
    eng::CfPtr<CFStringRef> ptr2{ nullptr };
    ptr2 = std::move(ptr1);

    ASSERT_FALSE(ptr1);
    ASSERT_EQ(kCFCompareEqualTo, ::CFStringCompare(ptr2.get(), CFSTR("hello"), 0));
}

