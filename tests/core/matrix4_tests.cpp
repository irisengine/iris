#include <array>
#include <cstring>

#include <gtest/gtest.h>

#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/real.h"
#include "core/vector3.h"

TEST(matrix4, constructor)
{
    eng::Matrix4 m{ };

    ASSERT_EQ(m[0], 1.0f);
    ASSERT_EQ(m[1], 0.0f);
    ASSERT_EQ(m[2], 0.0f);
    ASSERT_EQ(m[3], 0.0f);
    ASSERT_EQ(m[4], 0.0f);
    ASSERT_EQ(m[5], 1.0f);
    ASSERT_EQ(m[6], 0.0f);
    ASSERT_EQ(m[7], 0.0f);
    ASSERT_EQ(m[8], 0.0f);
    ASSERT_EQ(m[9], 0.0f);
    ASSERT_EQ(m[10], 1.0f);
    ASSERT_EQ(m[11], 0.0f);
    ASSERT_EQ(m[12], 0.0f);
    ASSERT_EQ(m[13], 0.0f);
    ASSERT_EQ(m[14], 0.0f);
    ASSERT_EQ(m[15], 1.0f);
}

TEST(matrix4, value_constructor)
{
    eng::Matrix4 m{ { {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f,
    } } };

    ASSERT_EQ(m[0], 1.0f);
    ASSERT_EQ(m[1], 2.0f);
    ASSERT_EQ(m[2], 3.0f);
    ASSERT_EQ(m[3], 4.0f);
    ASSERT_EQ(m[4], 5.0f);
    ASSERT_EQ(m[5], 6.0f);
    ASSERT_EQ(m[6], 7.0f);
    ASSERT_EQ(m[7], 8.0f);
    ASSERT_EQ(m[8], 9.0f);
    ASSERT_EQ(m[9], 10.0f);
    ASSERT_EQ(m[10], 11.0f);
    ASSERT_EQ(m[11], 12.0f);
    ASSERT_EQ(m[12], 13.0f);
    ASSERT_EQ(m[13], 14.0f);
    ASSERT_EQ(m[14], 15.0f);
    ASSERT_EQ(m[15], 16.0f);
}

TEST(matrix4, rotation_constructor)
{
    eng::Matrix4 m{ { 1.0f, 2.0f, 3.0f, 4.0f } };

    eng::Matrix4 expected { {
        -25.0f, -20.0f, 22.0f, 0.0f,
        28.0f, -19.0f, 4.0f, 0.0f,
        -10.0f, 20.0f, -9.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    } };

    ASSERT_EQ(m, expected);
}

TEST(matrix4, rotation_translation_constructor)
{
    eng::Matrix4 m{ { 1.0f, 2.0f, 3.0f, 4.0f }, { 1.0f, 2.0f, 3.0f } };

    eng::Matrix4 expected { {
        -25.0f, -20.0f, 22.0f, 1.0f,
        28.0f, -19.0f, 4.0f, 2.0f,
        -10.0f, 20.0f, -9.0f, 3.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    } };

    ASSERT_EQ(m, expected);
}

TEST(matrix4, equality)
{
    eng::Matrix4 m1{ { {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f,
    } } };

    auto m2 = m1;

    ASSERT_EQ(m1, m2);
}

TEST(matrix4, inequality)
{
    eng::Matrix4 m1{ { {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f,
    } } };

    auto m2 = m1;
    m2[0] = 100.0f;

    ASSERT_TRUE(m1 != m2);
}

TEST(matrix4, data)
{
    std::array<eng::real, 16> elements{ {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f,
    } };

    eng::Matrix4 m{ elements };

    ASSERT_EQ(std::memcmp(elements.data(), m.data(), sizeof(eng::real) * elements.size()), 0);
}

TEST(matrix4, column)
{
    eng::Matrix4 m{ { {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f,
    } } };

    ASSERT_EQ(m.column(0u), eng::Vector3(1.0f, 5.0f, 9.0f));
    ASSERT_EQ(m.column(1u), eng::Vector3(2.0f, 6.0f, 10.0f));
    ASSERT_EQ(m.column(2u), eng::Vector3(3.0f, 7.0f, 11.0f));
    ASSERT_EQ(m.column(3u), eng::Vector3(4.0f, 8.0f, 12.0f));
}

TEST(matrix4, matrix_multiplication)
{
    eng::Matrix4 m1{ { {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f,
    } } };

    eng::Matrix4 m2 = m1;
    auto m3 = m1 * m2;

    eng::Matrix4 expected{ { {
        90.0f, 100.0f, 110.0f, 120.0f,
        202.0f, 228.0f, 254.0f, 280.0f,
        314.0f, 356.0f, 398.0f, 440.0f,
        426.0f, 484.0f, 542.0f, 600.0f
    } } };

    ASSERT_EQ(m3, expected);
}

TEST(matrix4, matrix_multiplication_assignment)
{
    eng::Matrix4 m1{ { {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f,
    } } };

    eng::Matrix4 m2 = m1;
    m2 *= m1;

    eng::Matrix4 expected{ { {
        90.0f, 100.0f, 110.0f, 120.0f,
        202.0f, 228.0f, 254.0f, 280.0f,
        314.0f, 356.0f, 398.0f, 440.0f,
        426.0f, 484.0f, 542.0f, 600.0f
    } } };

    ASSERT_EQ(m2, expected);
}

TEST(matrix4, vector_multiplication)
{
    eng::Matrix4 m1{ { {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f,
    } } };

    auto m2 = m1 * eng::Vector3{ 1.1f, 2.2f, 3.3f };

    ASSERT_EQ(m2, eng::Vector3(19.4f, 49.8f, 80.2f));
}

TEST(matrix4, make_orthographic_projection)
{
    auto m = eng::Matrix4::make_orthographic_projection(100.0f, 100.0f, 100.0f);
    eng::Matrix4 expected{ { { 
        0.01f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.01f, 0.0f, 0.0f,
        0.0f, 0.0f, -0.01f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    } } };

    ASSERT_EQ(m, expected);
}

TEST(matrix4, make_perspective_projection)
{
    auto m = eng::Matrix4::make_perspective_projection(0.785398f, 1.0f, 100.0f, 0.1f, 100.0f);

    eng::Matrix4 expected{ { { 
        241.421402f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.414213896f, 0.0f, 0.0f,
        0.0f, 0.0f, -1.002002001f, -0.2002002001f,
        0.0f, 0.0f, -1.0f, 0.0f
    } } };

    ASSERT_EQ(m, expected);
}

TEST(matrix4, make_look_at)
{
    auto m = eng::Matrix4::make_look_at(
        { 1.0f, 2.0f, 3.0f },
        { },
        { 0.0f, 1.0f, 0.0f });

    eng::Matrix4 expected{ { { 
        0.9486833215f, 0.0f, -0.3162277937f, 5.960464478e-08f,
        -0.1690308601f, 0.8451542854f, -0.5070925355f, 0.0f,
        0.2672612369f, 0.5345224738f, 0.8017836809f, -3.741657257f,
        0.0f, 0.0f, 0.0f, 1.0f
    } } };

    ASSERT_EQ(m, expected);
}

TEST(matrix4, make_scale)
{
    auto m = eng::Matrix4::make_scale({ 1.0f, 2.0f, 3.0f });

    eng::Matrix4 expected{ { { 
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 3.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    } } };

    ASSERT_EQ(m, expected);
}

TEST(matrix4, make_translate)
{
    auto m = eng::Matrix4::make_translate({ 1.0f, 2.0f, 3.0f });

    eng::Matrix4 expected{ { { 
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 2.0f,
        0.0f, 0.0f, 1.0f, 3.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    } } };

    ASSERT_EQ(m, expected);
}

TEST(matrix4, transpose)
{
    eng::Matrix4 m{ { {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f,
    } } };

    eng::Matrix4 expected{ { {
        1.0f, 5.0f, 9.0f, 13.0f,
        2.0f, 6.0f, 10.0f, 14.0f,
        3.0f, 7.0f, 11.0f, 15.0f,
        4.0f, 8.0f, 12.0f, 16.0f,
    } } };

    ASSERT_EQ(eng::Matrix4::transpose(m), expected);
}

TEST(matrix4, invert)
{
    eng::Matrix4 m{ { {
        1.0f, 1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, 1.0f,
    } } };

    eng::Matrix4 expected{ { {
        0.25f, 0.25f, 0.25f, -0.25f,
        0.25f, 0.25f, -0.25f, 0.25f,
        0.25f, -0.25f, 0.25f, 0.25f,
        -0.25f, 0.25f, 0.25f, 0.25f,
    } } };

    ASSERT_EQ(eng::Matrix4::invert(m), expected);
}

TEST(matrix4, invert_round_trip)
{
    eng::Matrix4 m{ { {
        1.0f, 1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, 1.0f,
    } } };

    ASSERT_EQ(m * eng::Matrix4::invert(m), eng::Matrix4{ });
}

