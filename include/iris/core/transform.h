////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/vector3.h"

namespace iris
{

/**
 * Class representing transformation in 3D space. A transformation is formed of
 * a translation, rotation and scale. Conceptually this class provides a more
 * semantically specific api over a Matrix4.
 */
class Transform
{
  public:
    /**
     * Construct an empty Transform, this represents zero translation, zero
     * rotation and scale of 1.
     */
    Transform();

    /**
     * Construct a Transform from an existing Matrix4.
     *
     * @param matrix
     *   Matrix representing transform.
     */
    explicit Transform(const Matrix4 &matrix);

    /**
     * Construct a transform from component parts.
     *
     * @param translation
     *   Transform translation.
     *
     * @param rotation
     *   Transform rotation.
     *
     * @param scale
     *   Transform scale.
     */
    Transform(
        const Vector3 &translation,
        const Quaternion &rotation,
        const Vector3 &scale);

    /**
     * Get the matrix which represents this transformation.
     *
     * @returns
     *   Transformation matrix.
     */
    Matrix4 matrix() const;

    /**
     * Set matrix. Translation, rotation and scale will be derived from input.
     *
     * @param matrix
     *   New matrix.
     */
    void set_matrix(const Matrix4 &matrix);

    /**
     * Interpolate between this and another Transform.
     *
     * @param other
     *   Transform to interpolate to.
     *
     * @param amount
     *   Interpolation amount, must be in range [0.0, 1.0].
     */
    void interpolate(const Transform &other, float amount);

    /**
     * Get the translation component of the transform.
     *
     * @returns
     *   Translation component.
     */
    Vector3 translation() const;

    /**
     * Set translation.
     *
     * @param translation
     *   New translation.
     */
    void set_translation(const Vector3 &translation);

    /**
     * Get the rotation component of the transform.
     *
     * @returns
     *   Rotation component.
     */
    Quaternion rotation() const;

    /**
     * Set rotation.
     *
     * @param rotation
     *   New rotation.
     */
    void set_rotation(const Quaternion &rotation);

    /**
     * Get the scale component of the transform.
     *
     * @returns
     *   Scale component.
     */
    Vector3 scale() const;

    /**
     * Set scale.
     *
     * @param scale
     *   New scale.
     */
    void set_scale(const Vector3 &scale);

    /**
     * Equality operator.
     *
     * @param other
     *   Transform to compare for equality.
     *
     * @returns
     *   True if both transform are the same, false otherwise.
     */
    bool operator==(const Transform &other) const;

    /**
     * Inequality operator.
     *
     * @param other
     *   Transform to compare for inequality.
     *
     * @returns
     *   True if both transform are not the same, false otherwise.
     */
    bool operator!=(const Transform &other) const;

    /**
     * Performs transform multiplication. This results in a new transform, which
     * is this transform followed by the supplied one.
     *
     * @param other
     *   The transform to multiple.
     *
     * @returns
     *   New Transform which is this Transform multiplied by the supplied one.
     */
    Transform operator*(const Transform &other) const;

    /**
     * Performs transform multiplication. This results in a new transform, which
     * is this transform followed by the supplied one.
     *
     * @param other
     *   The transform to multiple.
     *
     * @returns
     *   This Transform after the supplied transform has been applied.
     */
    Transform &operator*=(const Transform &other);

    /**
     * Performs transform multiplication. This results in a new transform, which
     * is this transform followed by the supplied matrix.
     *
     * @param other
     *   The matrix to multiple.
     *
     * @returns
     *   This new Transform which is this transform multiplied by the supplied
     *   matrix.
     */
    Transform operator*(const Matrix4 &other) const;

    /**
     * Performs transform multiplication. This results in a new transform, which
     * is this transform followed by the supplied matrix.
     *
     * @param other
     *   The matrix to multiple.
     *
     * @returns
     *   This Transform after the supplied transform has been applied.
     */
    Transform &operator*=(const Matrix4 &other);

  private:
    /** Translation component. */
    Vector3 translation_;

    /** Rotation component. */
    Quaternion rotation_;

    /** Scale component. */
    Vector3 scale_;
};

}
