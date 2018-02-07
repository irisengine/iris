#include "matrix4.hpp"

#include <cmath>
#include <iostream>

#include "vector3.hpp"

namespace eng
{

matrix4::matrix4()
    : elements_({{ 1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f  }})
{ }

matrix4 matrix4::make_projection(
    const float fov,
    const float aspect_ratio,
    const float near,
    const float far) noexcept
{
    matrix4 m;

    const auto focal_length = 1.0f / std::tan(fov / 2.0f);

    m.elements_ = {{
        focal_length, 0.0f, 0.0f, 0.0f,
        0.0f, focal_length, 0.0f, 0.0f,
        0.0f, 0.0f, -(far + near) / (far - near), -(2 * far * near) / (far - near),
        0.0f, 0.0f, -1.0f, 0.0f
    }};

    return m;
}

matrix4 matrix4::make_look_at(
    const vector3 &eye,
    const vector3 &look_at,
    const vector3 &up) noexcept
{
    const auto f = vector3::normalise(look_at - eye);
    const auto up_normalised = vector3::normalise(up);

    const auto s = vector3::cross(f, up_normalised).normalise();
    const auto u = vector3::cross(s, f).normalise();

    matrix4 m;

    m.elements_ = {{
        s.x, s.y, s.z, 0.0f,
        u.x, u.y, u.z, 0.0f,
        -f.x, -f.y, -f.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    }};

    return m * make_translate(-eye);
}

matrix4 matrix4::make_scale(const vector3 &scale) noexcept
{
    matrix4 m;

    m.elements_ =
    {{
         scale.x, 0.0f, 0.0f, 0.0f,
         0.0f, scale.y, 0.0f, 0.0f,
         0.0f, 0.0f, scale.z, 0.0f,
         0.0f, 0.0f, 0.0f, 1.0f
    }};

    return m;
}

matrix4 matrix4::make_translate(const vector3 &translate) noexcept
{
    matrix4 m;

    m.elements_ =
    {{
         1.0f, 0.0f, 0.0f, translate.x,
         0.0f, 1.0f, 0.0f, translate.y,
         0.0f, 0.0f, 1.0f, translate.z,
         0.0f, 0.0f, 0.0f, 1.0f
    }};

    return m;
}

matrix4 matrix4::make_rotate_y(const float angle) noexcept
{
    matrix4 m;

    m.elements_ =
    {{
         std::cos(angle), 0.0f, std::sin(angle), 0.f,
         0.0f, 1.0f, 0.0f, 0.f,
         -std::sin(angle), 0.0f, std::cos(angle), 0.f,
         0.0f, 0.0f, 0.0f, 1.0f
    }};

    return m;
}

matrix4& matrix4::operator*=(const matrix4 &m) noexcept
{
    const auto e = elements_;

    const auto calculate_cell = [&e, &m](
        size_t row_num,
        size_t col_num)
    {
        return (e[row_num + 0u] * m[col_num +  0u]) +
               (e[row_num + 1u] * m[col_num +  4u]) +
               (e[row_num + 2u] * m[col_num +  8u]) +
               (e[row_num + 3u] * m[col_num + 12u]);
    };

    elements_[0u] = calculate_cell(0u, 0u);
    elements_[1u] = calculate_cell(0u, 1u);
    elements_[2u] = calculate_cell(0u, 2u);
    elements_[3u] = calculate_cell(0u, 3u);

    elements_[4u] = calculate_cell(4u, 0u);
    elements_[5u] = calculate_cell(4u, 1u);
    elements_[6u] = calculate_cell(4u, 2u);
    elements_[7u] = calculate_cell(4u, 3u);

    elements_[8u]  = calculate_cell(8u, 0u);
    elements_[9u]  = calculate_cell(8u, 1u);
    elements_[10u] = calculate_cell(8u, 2u);
    elements_[11u] = calculate_cell(8u, 3u);

    elements_[12u] = calculate_cell(12u, 0u);
    elements_[13u] = calculate_cell(12u, 1u);
    elements_[14u] = calculate_cell(12u, 2u);
    elements_[15u] = calculate_cell(12u, 3u);

    return *this;
}

matrix4 matrix4::operator*(const matrix4 &m) const noexcept
{
    return matrix4(*this) *= m;
}

float& matrix4::operator[](const size_t index) noexcept
{
    return elements_[index];
}

float matrix4::operator[](const size_t index) const noexcept
{
    return elements_[index];
}

const float* matrix4::data() const noexcept
{
    return elements_.data();
}

std::ostream& operator<<(std::ostream &out, const matrix4 &m) noexcept
{
    out << m[0] << " " << m[1] << " " <<  m[2] << " " << m[3] << std::endl;
    out << m[4] << " " << m[5] << " " <<  m[6] << " " << m[7] << std::endl;
    out << m[8] << " " << m[9] << " " <<  m[10] << " " << m[11] << std::endl;
    out << m[12] << " " << m[13] << " " <<  m[14] << " " << m[15];

    return out;
}

}

