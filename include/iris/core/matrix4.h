////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>
#include <cmath>
#include <ostream>

#include "core/quaternion.h"
#include "core/utils.h"
#include "core/vector3.h"

namespace iris
{

/**
 * Class represents a 4x4 matrix.
 *
 * This is a header only class to allow for constexpr methods.
 */
class Matrix4
{
  public:
    /**
     * Constructs a new identity Matrix4.
     */
    constexpr Matrix4()
        : elements_({{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}})
    {
    }

    /**
     * Constructs a new matrix with the supplied (row-major) values.
     *
     * @param elements
     *   Row major elements.
     */
    constexpr explicit Matrix4(const std::array<float, 16> &elements)
        : elements_(elements)
    {
    }

    /**
     * Construct a new Matrix4 which represents a rotation by the
     * supplied quaternion.
     *
     * @param rotation
     *   Rotation to represent.
     */
    constexpr explicit Matrix4(const Quaternion &rotation)
        : Matrix4()
    {
        elements_[0] = 1.0f - 2.0f * rotation.y * rotation.y - 2.0f * rotation.z * rotation.z;
        elements_[1] = 2.0f * rotation.x * rotation.y - 2.0f * rotation.z * rotation.w;
        elements_[2] = 2.0f * rotation.x * rotation.z + 2.0f * rotation.y * rotation.w;

        elements_[4] = 2.0f * rotation.x * rotation.y + 2.0f * rotation.z * rotation.w;
        elements_[5] = 1.0f - 2.0f * rotation.x * rotation.x - 2.0f * rotation.z * rotation.z;
        elements_[6] = 2.0f * rotation.y * rotation.z - 2.0f * rotation.x * rotation.w;

        elements_[8] = 2.0f * rotation.x * rotation.z - 2.0f * rotation.y * rotation.w;
        elements_[9] = 2.0f * rotation.y * rotation.z + 2.0f * rotation.x * rotation.w;
        elements_[10] = 1.0f - 2.0f * rotation.x * rotation.x - 2.0f * rotation.y * rotation.y;

        elements_[15] = 1.0f;
    }

    /**
     * Construct a new Matrix4 which represents a rotation and translation
     * by the supplied Quaternion and vector.
     *
     * @param rotation
     *   Rotation to represent.
     *
     * @param translation
     *   Translation to represent.
     */
    constexpr Matrix4(const Quaternion &rotation, const Vector3 &translation)
        : Matrix4(rotation)
    {
        elements_[3u] = translation.x;
        elements_[7u] = translation.y;
        elements_[11u] = translation.z;
    }

    /**
     * Static method to create an orthographic projection matrix.
     *
     * @param width
     *   width of window.
     *
     * @param height
     *   height of window.
     *
     * @param depth
     *   Depth of rendering view.
     *
     * @returns
     *   An orthographic projection matrix.
     */
    constexpr static Matrix4 make_orthographic_projection(float width, float height, float depth)
    {
        Matrix4 m{};

        const auto right = width;
        const auto left = -right;
        const auto top = height;
        const auto bottom = -top;
        const auto far_plane = depth;
        const auto near_plane = -far_plane;

        m.elements_ = {
            {2.0f / (right - left),
             0.0f,
             0.0f,
             -(right + left) / (right - left),
             0.0f,
             2.0f / (top - bottom),
             0.0f,
             -(top + bottom) / (top - bottom),
             0.0f,
             0.0f,
             -2.0f / (far_plane - near_plane),
             -(far_plane + near_plane) / (far_plane - near_plane),
             0.0f,
             0.0f,
             0.0f,
             1.0f}};

        return m;
    }

    /**
     * Static method to create a perspective projection matrix.
     *
     * @param fov
     *   Field of view.
     *
     * @param width
     *   Width of projection.
     *
     * @param height
     *   Height of projection.
     *
     * @param near_plane
     *   Near clipping plane.
     *
     * @param far_plane
     *   Far clipping plane.
     *
     * @returns
     *   A perspective projection matrix.
     */
    static Matrix4 make_perspective_projection(float fov, float width, float height, float near_plane, float far_plane)
    {
        Matrix4 m;

        const auto aspect_ratio = width / height;
        const auto tmp = ::tanf(fov / 2.0f);
        const auto t = tmp * near_plane;
        const auto b = -t;
        const auto r = t * aspect_ratio;
        const auto l = b * aspect_ratio;

        m.elements_ = {
            {(2.0f * near_plane) / (r - l),
             0.0f,
             (r + l) / (r - l),
             0.0f,
             0.0f,
             (2.0f * near_plane) / (t - b),
             (t + b) / (t - b),
             0.0f,
             0.0f,
             0.0f,
             -(far_plane + near_plane) / (far_plane - near_plane),
             -(2.0f * far_plane * near_plane) / (far_plane - near_plane),
             0.0f,
             0.0f,
             -1.0f,
             0.0f}};

        return m;
    }

    /**
     * Make a Matrix4 that can be used as a view matrix for a camera.
     *
     * @param eye
     *   Position of the camera.
     *
     * @param look_at
     *   The point where the camera is looking.
     *
     * @param up
     *   The up vector of the camera.
     *
     * @returns
     *   A Matrix4 that can be used as a camera view matrix.
     */
    static Matrix4 make_look_at(const Vector3 &eye, const Vector3 &look_at, const Vector3 &up)
    {
        const auto f = Vector3::normalise(look_at - eye);
        const auto up_normalised = Vector3::normalise(up);

        const auto s = Vector3::cross(f, up_normalised).normalise();
        const auto u = Vector3::cross(s, f).normalise();

        Matrix4 m;

        m.elements_ = {{s.x, s.y, s.z, 0.0f, u.x, u.y, u.z, 0.0f, -f.x, -f.y, -f.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};

        return m * make_translate(-eye);
    }

    /**
     * Static method to create a scale matrix.
     *
     * @param scale
     *   Vector3 specifying amount to scale along each axis.
     *
     * @returns
     *   Scale transformation matrix.
     */
    constexpr static Matrix4 make_scale(const Vector3 &scale)
    {
        Matrix4 m;

        m.elements_ = {
            {scale.x, 0.0f, 0.0f, 0.0f, 0.0f, scale.y, 0.0f, 0.0f, 0.0f, 0.0f, scale.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};

        return m;
    }

    /**
     * Static method to create translation matrix.
     *
     * @param translate
     *   Vector to translate by.
     */
    constexpr static Matrix4 make_translate(const Vector3 &translate)
    {
        Matrix4 m;

        m.elements_ = {
            {1.0f,
             0.0f,
             0.0f,
             translate.x,
             0.0f,
             1.0f,
             0.0f,
             translate.y,
             0.0f,
             0.0f,
             1.0f,
             translate.z,
             0.0f,
             0.0f,
             0.0f,
             1.0f}};

        return m;
    }

    /**
     * Invert a matrix. This produces a matrix such that:
     * M * invert(M) == Matrix4{ }
     *
     * @param m
     *   Matrix to invert.
     *
     * @returns
     *   Inverted matrix.
     */
    constexpr static Matrix4 invert(const Matrix4 &m)
    {
        Matrix4 inv{};

        inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] +
                 m[13] * m[6] * m[11] - m[13] * m[7] * m[10];

        inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] -
                 m[12] * m[6] * m[11] + m[12] * m[7] * m[10];

        inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] +
                 m[12] * m[5] * m[11] - m[12] * m[7] * m[9];

        inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] -
                  m[12] * m[5] * m[10] + m[12] * m[6] * m[9];

        inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] -
                 m[13] * m[2] * m[11] + m[13] * m[3] * m[10];

        inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] +
                 m[12] * m[2] * m[11] - m[12] * m[3] * m[10];

        inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] -
                 m[12] * m[1] * m[11] + m[12] * m[3] * m[9];

        inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] +
                  m[12] * m[1] * m[10] - m[12] * m[2] * m[9];

        inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] +
                 m[13] * m[2] * m[7] - m[13] * m[3] * m[6];

        inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] -
                 m[12] * m[2] * m[7] + m[12] * m[3] * m[6];

        inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] +
                  m[12] * m[1] * m[7] - m[12] * m[3] * m[5];

        inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] -
                  m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

        inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] -
                 m[9] * m[2] * m[7] + m[9] * m[3] * m[6];

        inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] +
                 m[8] * m[2] * m[7] - m[8] * m[3] * m[6];

        inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] -
                  m[8] * m[1] * m[7] + m[8] * m[3] * m[5];

        inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] +
                  m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

        auto det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

        if (det != 0.0f)
        {
            det = 1.0f / det;

            for (auto i = 0; i < 16; i++)
            {
                inv[i] *= det;
            }
        }

        return inv;
    }

    /**
     * Transpose a matrix.
     *
     * @param matrix
     *   Matrix to transpose.
     *
     * @returns
     *   Transposed matrix.
     */
    constexpr static Matrix4 transpose(const Matrix4 &matrix)
    {
        auto m{matrix};

        std::swap(m[1], m[4]);
        std::swap(m[2], m[8]);
        std::swap(m[3], m[12]);
        std::swap(m[6], m[9]);
        std::swap(m[7], m[13]);
        std::swap(m[11], m[14]);

        return m;
    }

    /**
     * Performs matrix multiplication.
     *
     * @param matrix
     *   The Matrix4 to multiply.
     *
     * @returns
     *   This Matrix4 multiplied the supplied Matrix4.
     */
    constexpr Matrix4 &operator*=(const Matrix4 &matrix)
    {
        const auto e = elements_;

        const auto calculate_cell = [&e, &matrix](std::size_t row_num, std::size_t col_num)
        {
            return (e[row_num + 0u] * matrix[col_num + 0u]) + (e[row_num + 1u] * matrix[col_num + 4u]) +
                   (e[row_num + 2u] * matrix[col_num + 8u]) + (e[row_num + 3u] * matrix[col_num + 12u]);
        };

        elements_[0u] = calculate_cell(0u, 0u);
        elements_[1u] = calculate_cell(0u, 1u);
        elements_[2u] = calculate_cell(0u, 2u);
        elements_[3u] = calculate_cell(0u, 3u);

        elements_[4u] = calculate_cell(4u, 0u);
        elements_[5u] = calculate_cell(4u, 1u);
        elements_[6u] = calculate_cell(4u, 2u);
        elements_[7u] = calculate_cell(4u, 3u);

        elements_[8u] = calculate_cell(8u, 0u);
        elements_[9u] = calculate_cell(8u, 1u);
        elements_[10u] = calculate_cell(8u, 2u);
        elements_[11u] = calculate_cell(8u, 3u);

        elements_[12u] = calculate_cell(12u, 0u);
        elements_[13u] = calculate_cell(12u, 1u);
        elements_[14u] = calculate_cell(12u, 2u);
        elements_[15u] = calculate_cell(12u, 3u);

        return *this;
    }

    /**
     * Performs Matrix4 multiplication.
     *
     * @param matrix
     *   The Matrix4 to multiply.
     *
     * @returns
     *   New Matrix4 which is this Matrix4 multiplied the supplied Matrix4.
     */
    constexpr Matrix4 operator*(const Matrix4 &matrix) const
    {
        return Matrix4(*this) *= matrix;
    }

    /**
     * Multiply this matrix by a given vector3.
     *
     * Internally this extends the Vector3 to have a fourth element with
     * a value of 1.0
     *
     * @param vector
     *   Vector3 to multiply by.
     *
     * @returns
     *   This matrix multiplied by the supplied vector3.
     */
    constexpr Vector3 operator*(const Vector3 &vector) const
    {
        return {
            vector.x * elements_[0] + vector.y * elements_[1] + vector.z * elements_[2] + elements_[3],

            vector.x * elements_[4] + vector.y * elements_[5] + vector.z * elements_[6] + elements_[7],

            vector.x * elements_[8] + vector.y * elements_[9] + vector.z * elements_[10] + elements_[11],
        };
    }

    /**
     * Get a reference to the element at the supplied index.
     *
     * @param index
     *   Index of element to get.
     *
     * @returns
     *   Reference to element at supplied index.
     */
    constexpr float &operator[](const size_t index)
    {
        return elements_[index];
    }

    /**
     * Get a copy of the element at the supplied index.
     *
     * @param index
     *   Index of element to get.
     *
     * @returns
     *   Copy of element at supplied index.
     */
    constexpr float operator[](const size_t index) const
    {
        return elements_[index];
    }

    /**
     * Equality operator.
     *
     * @param other
     *   Matrix4 to check for equality.
     *
     * @returns
     *   True if both Matrix4 objects are the same, false otherwise.
     */
    bool operator==(const Matrix4 &other) const
    {
        return std::equal(std::cbegin(elements_), std::cend(elements_), std::cbegin(other.elements_), compare);
    }

    /**
     * Inequality operator.
     *
     * @param other
     *   Matrix4 to check for inequality.
     *
     * @returns
     *   True if both Matrix4 objects are not the same, false otherwise.
     */
    bool operator!=(const Matrix4 &other) const
    {
        return !(*this == other);
    }

    /**
     * Get a pointer to the start of the internal Matrix4 data array.
     *
     * @returns
     *   Pointer to start if Matrix4 data.
     */
    constexpr const float *data() const
    {
        return elements_.data();
    }

    /**
     * Get a column from the matrix and return as a vector3. This ignores
     * the bottom row of the matrix.
     *
     * @param index
     *   The index of the column to return.
     *
     * @returns
     *   The first three value of the supplied column.
     */
    constexpr Vector3 column(const std::size_t index) const
    {
        return {elements_[index], elements_[index + 4u], elements_[index + 8u]};
    }

  private:
    /** Matrix4 data */
    std::array<float, 16u> elements_;
};

/**
 * Writes the Matrix4 to the stream, useful for debugging.
 *
 * @param out
 *   The stream to write to.
 *
 * @param m
 *   The Matrix4 to write to the stream.
 *
 * @return
 *   A reference to the supplied stream, after the Matrix4 has been
 *   written.
 */
inline std::ostream &operator<<(std::ostream &out, const Matrix4 &m)
{
    out << m[0] << " " << m[1] << " " << m[2] << " " << m[3] << std::endl;
    out << m[4] << " " << m[5] << " " << m[6] << " " << m[7] << std::endl;
    out << m[8] << " " << m[9] << " " << m[10] << " " << m[11] << std::endl;
    out << m[12] << " " << m[13] << " " << m[14] << " " << m[15];

    return out;
}

}
