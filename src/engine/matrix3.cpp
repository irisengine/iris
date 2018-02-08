#include "matrix3.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <utility>

#include "matrix4.hpp"
#include "vector3.hpp"

namespace eng
{

matrix3::matrix3() noexcept
    : matrix3({{
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f}})
{ }

matrix3::matrix3(const std::array<float, 9u> &elements) noexcept
    : elements_(elements)
{ }

matrix3::matrix3(const matrix4 &m) noexcept
    : matrix3({{
        m[0u], m[1u], m[2u],
        m[4u], m[5u], m[6u],
        m[8u], m[9u], m[10u]}})
{ }

matrix3& matrix3::operator*=(const matrix3 &m) noexcept
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

matrix3 matrix3::operator*(const matrix3 &m) const noexcept
{
    return matrix3{ *this } *= m;
}

vector3 matrix3::operator*(const vector3 &v) const noexcept
{
    return {
        v.x * elements_[0u] + v.y * elements_[1u] + v.z * elements_[2u],
        v.x * elements_[3u] + v.y * elements_[4u] + v.z * elements_[5u],
        v.x * elements_[6u] + v.y * elements_[7u] + v.z * elements_[8u]
    };
}

float& matrix3::operator[](const std::size_t index) noexcept
{
    return elements_[index];
}

float matrix3::operator[](const std::size_t index) const noexcept
{
    return elements_[index];
}

const float* matrix3::data() const noexcept
{
    return elements_.data();
}

matrix3& matrix3::invert() noexcept
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

        matrix3 inverse{ };

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

matrix3& matrix3::transpose() noexcept
{
    *this = matrix3{ {{
        elements_[0], elements_[3], elements_[6],
        elements_[1], elements_[4], elements_[7],
        elements_[2], elements_[5], elements_[8]
    }} };

    return *this;
}

matrix3 matrix3::invert(const matrix3 &m) noexcept
{
    return matrix3(m).invert();
}

matrix3 matrix3::transpose(const matrix3 &m) noexcept
{
    return matrix3(m).transpose();
}

std::ostream& operator<<(std::ostream &out, const matrix3 &m) noexcept
{
    out << m[0u] << " " << m[1u] << " " <<  m[2u] << std::endl;
    out << m[3u] << " " << m[4u] << " " <<  m[5u] << std::endl;
    out << m[6u] << " " << m[7u] << " " <<  m[8u];

    return out;
}

}

