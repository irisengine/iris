#pragma once

#include <array>
#include <iostream>

#include "quaternion.hpp"
#include "vector3.hpp"

namespace eng
{

/**
 * Class represents a 4x4 matrix.
 *
 * Elements are stored in row-major order.
 */
class matrix4
{
    public:

        /**
         * Constructs a new identity matrix4.
         */
        matrix4();

        /**
         * Constructs a new matrix with the supplied (row-major) values.
         *
         * @param elements
         *   Row major elements.
         */
        explicit matrix4(const std::array<float, 16> &elements);

        /**
         * Construct a new matrix4 which represents a rotation by the
         * supplied quaternion.
         *
         * @param rotation
         *   Rotation to represent.
         */
        explicit matrix4(const quaternion &rotation);

        /**
         * Construct a new matrix4 which represents a rotation and translation
         * by the supplied quaternion and vector.
         *
         * @param rotation
         *   Rotation to represent.
         *
         * @param translation
         *   Translation to represent.
         */
        matrix4(const quaternion &rotation, const vector3 &translation);

        /**
         * Static method to create a projection matrix.
         *
         * @param fov
         *   Field of view.
         *
         * @param aspect_ration
         *   Projection aspect ratio.
         *
         * @param near
         *   Near clip plane.
         *
         * @far
         *   Far clip plane.
         */
        static matrix4 make_projection(
            const float fov,
            const float aspect_ratio,
            const float near,
            const float far);

        /**
         * Make a matrix4 that can be used as a view matrix for a camera.
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
         *   A matrix4 that can be used as a camera view matrix.
         */
        static matrix4 make_look_at(
            const vector3 &eye,
            const vector3 &look_at,
            const vector3 &up);

        /**
         * Static method to create a scale matrix.
         *
         * @param scale
         *   Vector3 specifying amount to scale along each axis.
         *
         * @returns
         *   Scale transformation matrix.
         */
        static matrix4 make_scale(const vector3 &scale);

        /**
         * Static method to create translation matrix.
         *
         * @param translate
         *   Vector to translate by.
         */
        static matrix4 make_translate(const vector3 &translate);

        /**
         * Static method to create a rotation matrix about the y axis.
         *
         * @param angle
         *   Angle to rotate by in radians.
         */
        static matrix4 make_rotate_y(const float angle);

        /**
         * Performs matrix multiplication.
         *
         * @param m
         *   The matrix4 to multiply.
         *
         * @returns
         *   This matrix4 multiplied the supplied matrix4.
         */
        matrix4& operator*=(const matrix4 &m);

        /**
         * Performs matrix4 multiplication.
         *
         * @param m
         *   The matrix4 to multiply.
         *
         * @returns
         *   New matrix4 which is this matrix4 multiplied the supplied matrix4.
         */
        matrix4 operator*(const matrix4 &m) const;

        /**
         * Multiply this matrix by a given vector3.
         *
         * Internally this extends the vector3 to have a fourth element with
         * a value of 1.0
         *
         * @param v
         *   vector3 to multiply by.
         *
         * @returns
         *   This matrix multiplied by the supplied vector3.
         */
        vector3 operator*(const vector3 &v) const;

        /**
         * Get a reference to the element at the supplied index.
         *
         * @param index
         *   Index of element to get.
         *
         * @returns
         *   Reference to element at supplied index.
         */
        float& operator[](const size_t index);

        /**
         * Get a copy of the element at the supplied index.
         *
         * @param index
         *   Index of element to get.
         *
         * @returns
         *   Copy of element at supplied index.
         */
        float operator[](const size_t index) const;

        /**
         * Get a pointer to the start of the internal matrix4 data array.
         *
         * @returns
         *   Pointer to start if matrix4 data.
         */
        const float* data() const;

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
        vector3 column(const std::size_t index) const;

        /**
         * Writes the matrix4 to the stream, useful for debugging.
         *
         * @param out
         *   The stream to write to.
         *
         * @param out
         *   The matrix4 to write to the stream.
         *
         * @return
         *   A reference to the supplied stream, after the matrix4 has been
         *   written.
         */
        friend std::ostream& operator<<( std::ostream &out, const matrix4 &m);

    private:

        /** matrix4 data */
        std::array<float, 16u> elements_;
};

}

