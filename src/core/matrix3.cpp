#include "core/matrix3.hpp"

#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <utility>

#include "core/matrix4.hpp"
#include "core/vector3.hpp"

namespace eng
{

Matrix3::Matrix3()
    : Matrix3({{
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f}})
{ }

Matrix3::Matrix3(const std::array<float, 9u> &elements)
    : elements_(elements)
{ }

Matrix3::Matrix3(const Matrix4 &m)
    : Matrix3({{
        m[0u], m[1u], m[2u],
        m[4u], m[5u], m[6u],
        m[8u], m[9u], m[10u]}})
{ }

Matrix3& Matrix3::operator*=(const Matrix3 &m)
{
    const auto e = elements_;

    // helper lambda to calculate a single cell of a multiplied matrix
    const auto calculate_cell = [&e, &m](
        size_t row_num,
        size_t col_num)
    {
        return (e[row_num + 0u] * m[col_num + 0u]) +
               (e[row_num + 1u] * m[col_num + 3u]) +
               (e[row_num + 2u] * m[col_num + 6u]);
    };

    elements_[0u] = calculate_cell(0u, 0u);
    elements_[1u] = calculate_cell(0u, 1u);
    elements_[2u] = calculate_cell(0u, 2u);

    elements_[3u] = calculate_cell(3u, 0u);
    elements_[4u] = calculate_cell(3u, 1u);
    elements_[5u] = calculate_cell(3u, 2u);

    elements_[6u] = calculate_cell(6u, 0u);
    elements_[7u] = calculate_cell(6u, 1u);
    elements_[8u] = calculate_cell(6u, 2u);

    return *this;
}

Matrix3 Matrix3::operator*(const Matrix3 &m) const
{
    return Matrix3{ *this } *= m;
}

Vector3 Matrix3::operator*(const Vector3 &v) const
{
    return {
        v.x * elements_[0u] + v.y * elements_[1u] + v.z * elements_[2u],
        v.x * elements_[3u] + v.y * elements_[4u] + v.z * elements_[5u],
        v.x * elements_[6u] + v.y * elements_[7u] + v.z * elements_[8u]
    };
}

Matrix3& Matrix3::operator*=(const float s)
{
    for(auto &e : elements_)
    {
        e *= s;
    }

    return *this;
}

Matrix3 Matrix3::operator*(const float s) const
{
    return Matrix3{ *this } *= s;
}

float& Matrix3::operator[](const std::size_t index)
{
    return elements_[index];
}

Matrix3& Matrix3::operator+=(const Matrix3 &m)
{
    for(auto i = 0u; i < elements_.size(); ++i)
    {
        elements_[i] += m[i];
    }

    return *this;
}

Matrix3 Matrix3::operator+(const Matrix3 &m) const
{
    return Matrix3{ *this } += m;
}

float Matrix3::operator[](const std::size_t index) const
{
    return elements_[index];
}

const float* Matrix3::data() const
{
    return elements_.data();
}

Matrix3& Matrix3::invert()
{
    const auto determinant =
        elements_[0u] * elements_[4u] * elements_[8u] +
        elements_[1u] * elements_[5u] * elements_[6u] +
        elements_[2u] * elements_[3u] * elements_[7u] -
        elements_[2u] * elements_[4u] * elements_[6u] -
        elements_[1u] * elements_[3u] * elements_[8u] -
        elements_[0u] * elements_[5u] * elements_[7u];

    if(determinant != 0.0f)
    {
        const auto t = transpose(*this);

        // helper lambda to calculate a minor determinant
        const auto minor_determinant = [&t](
            const std::uint32_t index1,
            const std::uint32_t index2,
            const std::uint32_t index3,
            const std::uint32_t index4)
        {
            return (t[index1] * t[index4]) -
                   (t[index2] * t[index3]);
        };

        Matrix3 inverse{ };

        inverse[0u] =  minor_determinant(4u, 5u, 7u, 8u) / determinant;
        inverse[1u] = -minor_determinant(3u, 5u, 6u, 8u) / determinant;
        inverse[2u] =  minor_determinant(3u, 4u, 6u, 7u) / determinant;
        inverse[3u] = -minor_determinant(1u, 2u, 7u, 8u) / determinant;
        inverse[4u] =  minor_determinant(0u, 2u, 6u, 8u) / determinant;
        inverse[5u] = -minor_determinant(0u, 1u, 6u, 7u) / determinant;
        inverse[6u] =  minor_determinant(1u, 2u, 4u, 5u) / determinant;
        inverse[7u] = -minor_determinant(0u, 2u, 3u, 5u) / determinant;
        inverse[8u] =  minor_determinant(0u, 1u, 3u, 4u) / determinant;

        *this = std::move(inverse);
    }

    return *this;
}

Matrix3& Matrix3::transpose()
{
    *this = Matrix3{ {{
        elements_[0], elements_[3], elements_[6],
        elements_[1], elements_[4], elements_[7],
        elements_[2], elements_[5], elements_[8]
    }} };

    return *this;
}

Matrix3 Matrix3::invert(const Matrix3 &m)
{
    return Matrix3(m).invert();
}

Matrix3 Matrix3::transpose(const Matrix3 &m)
{
    return Matrix3(m).transpose();
}

Matrix3 Matrix3::make_orthonormal_basis(const Vector3 &x)
{
    // we needed to generate a y and z axis, we take a guess at one of those
    // by using either world y or world x, which ever is furthest from our
    // supplied x
    const auto guess = std::abs(x.x) > std::abs(x.y)
        ? Vector3(0.0f, 1.0f, 0.0f)
        : Vector3(1.0f, 0.0f, 0.0f);

    // calculate z axis, which is guaranteed to be orthogonal to supplied x
    auto z = Vector3::cross(x, guess);

    // we now have x and z, so calculate y
    auto y = Vector3::cross(z, x);

    // ensure normalised
    z.normalise();
    y.normalise();

    // create transform matrix
    return Matrix3{{{
        x.x, y.x, z.x,
        x.y, y.y, z.y,
        x.z, y.z, z.z,
    }}};
}

Matrix3 Matrix3::make_skew_symmetric(const Vector3 &v)
{
    return Matrix3{{{
        0.0f, -v.z, v.y,
        v.z, 0.0f, -v.x,
        -v.y, v.x, 0.0f
    }}};
}

std::ostream& operator<<(std::ostream &out, const Matrix3 &m)
{
    out << m[0u] << " " << m[1u] << " " <<  m[2u] << std::endl;
    out << m[3u] << " " << m[4u] << " " <<  m[5u] << std::endl;
    out << m[6u] << " " << m[7u] << " " <<  m[8u];

    return out;
}

}

