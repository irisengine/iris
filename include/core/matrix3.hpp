#pragma once

#include <array>
#include <cstddef>
#include <iosfwd>

#include "matrix4.hpp"
#include "vector3.hpp"

namespace eng
{

/**
 * Class represents a 3x3 matrix.
 *
 * Elements are in row-major order.
 */
class Matrix3
{
    public:

        /**
         * Constructs a new identity matrix.
         */
        Matrix3();

        /**
         * Constructs a new matrix with the supplied (row-major) values.
         *
         * @param elements
         *   Row major elements.
         */
        explicit Matrix3(const std::array<float, 9u> &elements);

        /**
         * Construct a new Matrix3 from a matrix4.
         *
         * This takes the upper right set of values from the matrix4. Can be
         * to remove the position information from a 4x4 transformation
         * matrix.
         */
        explicit Matrix3(const Matrix4 &m);

        /**
         * Performs matrix multiplication.
         *
         * @param m
         *   The Matrix3 to multiply.
         *
         * @returns
         *   This Matrix3 multiplied the supplied Matrix3.
         */
        Matrix3& operator*=(const Matrix3 &m);

        /**
         * Performs Matrix3 multiplication.
         *
         * @param m
         *   The Matrix3 to multiply.
         *
         * @returns
         *   New Matrix3 which is this Matrix3 multiplied the supplied Matrix3.
         */
        Matrix3 operator*(const Matrix3 &m) const;

        /**
         * Multiply this Matrix3 by a vector3.
         *
         * @params v
         *   The vector to multiply by.
         *
         *  @returns
         *    This multiplied by v.
         */
        Vector3 operator*(const Vector3 &v) const;

        /**
         * Multiply each element in the matrix by a scalar value.
         *
         * @param s
         *   Scalar value to multiply by.
         *
         * @returns
         *   This Matrix3 with each element multiplied by scalar value.
         */
        Matrix3& operator*=(const float s);

        /**
         * Multiply each element in the matrix by a scalar value.
         *
         * @param s
         *   Scalar value to multiply by.
         *
         * @returns
         *   New Matrix3 which is this Matrix3 with each element multiplied by
         *   scalar value
         */
        Matrix3 operator*(const float s) const;

        /**
         * Perform matrix additions.
         *
         * @param m
         *   Matrix3 to add to this.
         *
         * @returns
         *   This Matrix3 added to the supplied Matrix3.
         */
        Matrix3& operator+=(const Matrix3 &m);

        /**
         * Perform matrix additions.
         *
         * @param m
         *   Matrix3 to add to this.
         *
         * @returns
         *   New Matrix3 which is this Matrix3 added to the supplied Matrix3.
         */
        Matrix3 operator+(const Matrix3 &m) const;

        /**
         * Get a reference to the element at the supplied index.
         *
         * @param index
         *   Index of element to get.
         *
         * @returns
         *   Reference to element at supplied index.
         */
        float& operator[](const std::size_t index);

        /**
         * Get a copy of the element at the supplied index.
         *
         * @param index
         *   Index of element to get.
         *
         * @returns
         *   Copy of element at supplied index.
         */
        float operator[](const std::size_t index) const;

        /**
         * Get a pointer to the start of the internal Matrix3 data array.
         *
         * @returns
         *   Pointer to start if Matrix3 data.
         */
        const float* data() const;

        /**
         * Inverts this matrix.
         *
         * @returns
         *   A reference to this Matrix3.
         */
        Matrix3& invert();

        /**
         * Transposes this matrix.
         *
         * @returns
         *   A reference to this Matrix3.
         */
        Matrix3& transpose();

        /**
         * Static method to invert a matrix.
         *
         * @param m
         *   Matrix to invert.
         *
         * @returns
         *   A copy of the supplied matrix inverted.
         */
        static Matrix3 invert(const Matrix3 &m);

        /**
         * Static method to transpose a matrix.
         *
         * @param m
         *   Matrix to transpose.
         *
         * @returns
         *   A copy of the supplied matrix transposed.
         */
        static Matrix3 transpose(const Matrix3 &m);

        /**
         * Make an orthonormal basis from a starting vector. This will create
         * an orthogonal y and z axis for the supplied x and the resulting
         * matrix will transform points into the orthonormal basis.
         *
         * @param x
         *   x axis of the orthonormal basis.
         *
         * @returns
         *   Transform matrix which will transform points to orthonormal basis.
         */
        static Matrix3 make_orthonormal_basis(const Vector3 &x);

        /**
         * Static method to make a skew-symmetric matrix, i.e. a matrix whose
         * transpose is equal to its negative. This takes a vector as a
         * skew-symmetric matrix can be used to compute a cross product.
         *
         * @param v
         *   Vector3 from which values will be used so that a cross product
         *   can be calculated.
         *
         * @returns
         *   Skew-symmetric matrix.
         */
        static Matrix3 make_skew_symmetric(const Vector3 &v);

        /**
         * Writes the Matrix3 to the stream, useful for debugging.
         *
         * @param out
         *   The stream to write to.
         *
         * @param out
         *   The Matrix3 to write to the stream.
         *
         * @return
         *   A reference to the supplied stream, after the Matrix3 has been
         *   written.
         */
        friend std::ostream& operator<<(std::ostream &out, const Matrix3 &m);

    private:

        /** Matrix3 data. */
        std::array<float, 9u> elements_;
};

}

