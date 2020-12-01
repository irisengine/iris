#include "core/transform.h"

#include <cmath>

#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/vector3.h"

/**
 * Convert a transformation matrix into translation, rotation and scale
 * components.
 *
 * @param matrix
 *   Matrix to decompose.
 *
 * @returns
 *   Tuple of <translation, rotation, scale>
 */
std::tuple<iris::Vector3, iris::Quaternion, iris::Vector3> decompose(
    iris::Matrix4 matrix)
{
    // extract translation
    const iris::Vector3 translation = matrix.column(3u);

    // extract scale
    const iris::Vector3 scale = {
        matrix.column(0u).magnitude(),
        matrix.column(1u).magnitude(),
        matrix.column(2u).magnitude()};

    // convert upper left 3x3 matrix to rotation matrix
    matrix[0] /= scale.x;
    matrix[1] /= scale.y;
    matrix[2] /= scale.z;
    matrix[4] /= scale.x;
    matrix[5] /= scale.y;
    matrix[6] /= scale.z;
    matrix[8] /= scale.x;
    matrix[9] /= scale.y;
    matrix[10] /= scale.z;

    matrix[3] = 0.0f;
    matrix[7] = 0.0f;
    matrix[11] = 0.0f;

    iris::Quaternion rotation{};

    // the following code is cribbed from OgreQuaternion.cpp FromRotatinMatrix
    // commit: e1c3732c51f9099bed10d36805b738015adc8f47
    // which in turn is based on:
    //  Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    //  article "Quaternion Calculus and Fast Animation".

    const auto trace = matrix[0] + matrix[5] + matrix[10];
    float root = 0.0f;

    if (trace > 0.0f)
    {
        root = std::sqrt(trace + 1.0f);
        rotation.w = 0.5f * root;
        root = 0.5f / root;
        rotation.x = (matrix[9] - matrix[6]) * root;
        rotation.y = (matrix[2] - matrix[8]) * root;
        rotation.z = (matrix[4] - matrix[1]) * root;
    }
    else
    {
        static std::size_t next[3] = {1, 2, 0};
        std::size_t i = 0;

        if (matrix[5] > matrix[0])
        {
            i = 1;
        }

        if (matrix[10] > matrix[(i * 4u) + i])
        {
            i = 2;
        }

        const auto j = next[i];
        const auto k = next[j];

        root = std::sqrt(
            matrix[(i * 4u) + i] - matrix[(j * 4u) + j] - matrix[(k * 4u) + k] +
            1.0f);

        float *quat[3] = {&rotation.x, &rotation.y, &rotation.z};

        *quat[i] = 0.5f * root;
        root = 0.5f / root;
        rotation.w = (matrix[(k * 4u) + j] - matrix[(j * 4u) + k]) * root;
        *quat[j] = (matrix[(j * 4u) + i] + matrix[(i * 4u) + j]) * root;
        *quat[k] = (matrix[(k * 4u) + i] + matrix[(i * 4u) + k]) * root;
    }

    return {translation, rotation, scale};
}

namespace iris
{

Transform::Transform()
    : Transform({}, {}, {1.0f})
{
}

Transform::Transform(const Matrix4 &matrix)
    : Transform({0.0f}, {}, {0.0f})
{
    const auto [translation, rotation, scale] = decompose(matrix);

    translation_ = translation;
    rotation_ = rotation;
    scale_ = scale;
}

Transform::Transform(
    const Vector3 &translation,
    const Quaternion &rotation,
    const Vector3 &scale)
    : translation_(translation)
    , rotation_(rotation)
    , scale_(scale)
{
}

Matrix4 Transform::matrix() const
{
    return Matrix4::make_translate(translation_) * Matrix4(rotation_) *
           Matrix4::make_scale(scale_);
}

void Transform::set_matrix(const Matrix4 &matrix)
{
    const auto [translation, rotation, scale] = decompose(matrix);

    translation_ = translation;
    rotation_ = rotation;
    scale_ = scale;
}

void Transform::interpolate(const Transform &other, float amount)
{
    translation_.lerp(other.translation_, amount);
    rotation_.slerp(other.rotation_, amount);
    scale_.lerp(other.scale_, amount);
}

Vector3 Transform::translation() const
{
    return translation_;
}

void Transform::set_translation(const Vector3 &translation)
{
    translation_ = translation;
}

Quaternion Transform::rotation() const
{
    return rotation_;
}

void Transform::set_rotation(const Quaternion &rotation)
{
    rotation_ = rotation;
}

Vector3 Transform::scale() const
{
    return scale_;
}

void Transform::set_scale(const Vector3 &scale)
{
    scale_ = scale;
}

bool Transform::operator==(const Transform &other) const
{
    return (translation_ == other.translation_) &&
           (rotation_ == other.rotation_) && (scale_ == other.scale_);
}

bool Transform::operator!=(const Transform &other) const
{
    return !(*this == other);
}

Transform Transform::operator*(const Transform &other) const
{
    return Transform{*this} *= other;
}

Transform &Transform::operator*=(const Transform &other)
{
    return *this *= other.matrix();
}

Transform Transform::operator*(const Matrix4 &other) const
{
    return Transform{*this} *= other;
}

Transform &Transform::operator*=(const Matrix4 &other)
{
    auto new_matrix = matrix() * other;

    const auto [translation, rotation, scale] = decompose(new_matrix);

    translation_ = translation;
    rotation_ = rotation;
    scale_ = scale;

    return *this;
}

}
