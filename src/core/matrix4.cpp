#include "core/matrix4.hpp"

#include <cmath>
#include <iostream>

#include "core/quaternion.hpp"
#include "core/vector3.hpp"

namespace eng
{

matrix4::matrix4()
    : elements_({{ 1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f  }})
{ }

matrix4::matrix4(const std::array<float, 16> &elements)
    : elements_(elements)
{ }

matrix4::matrix4(const quaternion &q)
    : matrix4()
{
    elements_[0] = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
    elements_[1] = 2.0f * q.x * q.y - 2.0f * q.z * q.w;
    elements_[2] = 2.0f * q.x * q.z + 2.0f * q.y * q.w;

    elements_[4] = 2.0f * q.x * q.y + 2.0f * q.z * q.w;
    elements_[5] = 1.0f - 2.0f * q.x * q.x  - 2.0f * q.z * q.z;
    elements_[6] = 2.0f * q.y * q.z - 2.0f * q.x * q.w;

    elements_[8] = 2.0f * q.x * q.z - 2.0f * q.y * q.w;
    elements_[9] = 2.0f * q.y * q.z + 2.0f * q.x * q.w;
    elements_[10] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;
}

matrix4::matrix4(const quaternion &q, const vector3 &p)
    : matrix4(q)
{
    elements_[3u] = p.x;
    elements_[7u] = p.y;
    elements_[11u] = p.z;
}

matrix4 matrix4::make_projection(
    const float fov,
    const float aspect_ratio,
    const float near,
    const float far)
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
    const vector3 &up)
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

matrix4 matrix4::make_scale(const vector3 &scale)
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

matrix4 matrix4::make_translate(const vector3 &translate)
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

matrix4 matrix4::make_rotate_y(const float angle)
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

matrix4& matrix4::operator*=(const matrix4 &m)
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

matrix4 matrix4::operator*(const matrix4 &m) const
{
    return matrix4(*this) *= m;
}

vector3 matrix4::operator*(const vector3 &v) const
{
    return {
        v.x * elements_[0] +
        v.y * elements_[1] +
        v.z * elements_[2] +
        elements_[3],

        v.x * elements_[4] +
        v.y * elements_[5] +
        v.z * elements_[6] +
        elements_[7],

        v.x * elements_[8] +
        v.y * elements_[9] +
        v.z * elements_[10] +
        elements_[11],
    };
}

float& matrix4::operator[](const size_t index)
{
    return elements_[index];
}

float matrix4::operator[](const size_t index) const
{
    return elements_[index];
}

const float* matrix4::data() const
{
    return elements_.data();
}

vector3 matrix4::column(const std::size_t index) const
{
    return{ elements_[index], elements_[index + 4u], elements_[index + 8u] };
}

std::ostream& operator<<(std::ostream &out, const matrix4 &m)
{
    out << m[0] << " " << m[1] << " " <<  m[2] << " " << m[3] << std::endl;
    out << m[4] << " " << m[5] << " " <<  m[6] << " " << m[7] << std::endl;
    out << m[8] << " " << m[9] << " " <<  m[10] << " " << m[11] << std::endl;
    out << m[12] << " " << m[13] << " " <<  m[14] << " " << m[15];

    return out;
}

}

