#include "core/matrix4.hpp"

#include <cmath>
#include <iostream>

#include "core/quaternion.hpp"
#include "core/vector3.hpp"

namespace eng
{

Matrix4::Matrix4()
    : elements_({{ 1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f  }})
{ }

Matrix4::Matrix4(const std::array<float, 16> &elements)
    : elements_(elements)
{ }

Matrix4::Matrix4(const Quaternion &q)
    : Matrix4()
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

Matrix4::Matrix4(const Quaternion &q, const Vector3 &p)
    : Matrix4(q)
{
    elements_[3u] = p.x;
    elements_[7u] = p.y;
    elements_[11u] = p.z;
}

Matrix4 Matrix4::make_projection(
    const float fov,
    const float aspect_ratio,
    const float near,
    const float far)
{
    Matrix4 m;

    const auto focal_length = 1.0f / std::tan(fov / 2.0f);

    m.elements_ = {{
        focal_length, 0.0f, 0.0f, 0.0f,
        0.0f, focal_length, 0.0f, 0.0f,
        0.0f, 0.0f, -(far + near) / (far - near), -(2 * far * near) / (far - near),
        0.0f, 0.0f, -1.0f, 0.0f
    }};

    return m;
}

Matrix4 Matrix4::make_look_at(
    const Vector3 &eye,
    const Vector3 &look_at,
    const Vector3 &up)
{
    const auto f = Vector3::normalise(look_at - eye);
    const auto up_normalised = Vector3::normalise(up);

    const auto s = Vector3::cross(f, up_normalised).normalise();
    const auto u = Vector3::cross(s, f).normalise();

    Matrix4 m;

    m.elements_ = {{
        s.x, s.y, s.z, 0.0f,
        u.x, u.y, u.z, 0.0f,
        -f.x, -f.y, -f.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    }};

    return m * make_translate(-eye);
}

Matrix4 Matrix4::make_scale(const Vector3 &scale)
{
    Matrix4 m;

    m.elements_ =
    {{
         scale.x, 0.0f, 0.0f, 0.0f,
         0.0f, scale.y, 0.0f, 0.0f,
         0.0f, 0.0f, scale.z, 0.0f,
         0.0f, 0.0f, 0.0f, 1.0f
    }};

    return m;
}

Matrix4 Matrix4::make_translate(const Vector3 &translate)
{
    Matrix4 m;

    m.elements_ =
    {{
         1.0f, 0.0f, 0.0f, translate.x,
         0.0f, 1.0f, 0.0f, translate.y,
         0.0f, 0.0f, 1.0f, translate.z,
         0.0f, 0.0f, 0.0f, 1.0f
    }};

    return m;
}

Matrix4 Matrix4::make_rotate_y(const float angle)
{
    Matrix4 m;

    m.elements_ =
    {{
         std::cos(angle), 0.0f, std::sin(angle), 0.f,
         0.0f, 1.0f, 0.0f, 0.f,
         -std::sin(angle), 0.0f, std::cos(angle), 0.f,
         0.0f, 0.0f, 0.0f, 1.0f
    }};

    return m;
}

Matrix4& Matrix4::operator*=(const Matrix4 &matrix)
{
    const auto e = elements_;

    const auto calculate_cell = [&e, &matrix](
        size_t row_num,
        size_t col_num)
    {
        return (e[row_num + 0u] * matrix[col_num +  0u]) +
               (e[row_num + 1u] * matrix[col_num +  4u]) +
               (e[row_num + 2u] * matrix[col_num +  8u]) +
               (e[row_num + 3u] * matrix[col_num + 12u]);
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

Matrix4 Matrix4::operator*(const Matrix4 &matrix) const
{
    return Matrix4(*this) *= matrix;
}

Vector3 Matrix4::operator*(const Vector3 &vector) const
{
    return {
        vector.x * elements_[0] +
        vector.y * elements_[1] +
        vector.z * elements_[2] +
        elements_[3],

        vector.x * elements_[4] +
        vector.y * elements_[5] +
        vector.z * elements_[6] +
        elements_[7],

        vector.x * elements_[8] +
        vector.y * elements_[9] +
        vector.z * elements_[10] +
        elements_[11],
    };
}

float& Matrix4::operator[](const size_t index)
{
    return elements_[index];
}

float Matrix4::operator[](const size_t index) const
{
    return elements_[index];
}

const float* Matrix4::data() const
{
    return elements_.data();
}

Vector3 Matrix4::column(const std::size_t index) const
{
    return{ elements_[index], elements_[index + 4u], elements_[index + 8u] };
}

std::ostream& operator<<(std::ostream &out, const Matrix4 &m)
{
    out << m[0] << " " << m[1] << " " <<  m[2] << " " << m[3] << std::endl;
    out << m[4] << " " << m[5] << " " <<  m[6] << " " << m[7] << std::endl;
    out << m[8] << " " << m[9] << " " <<  m[10] << " " << m[11] << std::endl;
    out << m[12] << " " << m[13] << " " <<  m[14] << " " << m[15];

    return out;
}

}

