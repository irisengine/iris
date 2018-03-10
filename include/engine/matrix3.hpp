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
class matrix3 final
{
    public:

        /**
         * Constructs a new identity matrix.
         */
        matrix3() noexcept;

        /**
         * Constructs a new matrix with the supplied (row-major) values.
         *
         * @param elements
         *   Row major elements.
         */
        explicit matrix3(const std::array<float, 9u> &elements) noexcept;

        /**
         * Construct a new matrix3 from a matrix4.
         *
         * This takes the upper right set of values from the matrix4. Can be
         * to remove the position information from a 4x4 transformation
         * matrix.
         */
        explicit matrix3(const matrix4 &m) noexcept;

        /**
         * Performs matrix multiplication.
         *
         * @param m
         *   The matrix3 to multiply.
         *
         * @returns
         *   This matrix3 multiplied the supplied matrix3.
         */
        matrix3& operator*=(const matrix3 &m) noexcept;

        /**
         * Performs matrix3 multiplication.
         *
         * @param m
         *   The matrix3 to multiply.
         *
         * @returns
         *   New matrix3 which is this matrix3 multiplied the supplied matrix3.
         */
        matrix3 operator*(const matrix3 &m) const noexcept;

        /**
         * Multiply this matrix3 by a vector3.
         *
         * @params v
         *   The vector to multiply by.
         *
         *  @returns
         *    This multiplied by v.
         */
        vector3 operator*(const vector3 &v) const noexcept;

        /**
         * Multiply each element in the matrix by a scalar value.
         *
         * @param s
         *   Scalar value to multiply by.
         *
         * @returns
         *   This matrix3 with each element multiplied by scalar value.
         */
        matrix3& operator*=(const float s) noexcept;

        /**
         * Multiply each element in the matrix by a scalar value.
         *
         * @param s
         *   Scalar value to multiply by.
         *
         * @returns
         *   New matrix3 which is this matrix3 with each element multiplied by
         *   scalar value
         */
        matrix3 operator*(const float s) const noexcept;

        /**
         * Perform matrix additions.
         *
         * @param m
         *   matrix3 to add to this.
         *
         * @returns
         *   This matrix3 added to the supplied matrix3.
         */
        matrix3& operator+=(const matrix3 &m) noexcept;

        /**
         * Perform matrix additions.
         *
         * @param m
         *   matrix3 to add to this.
         *
         * @returns
         *   New matrix3 which is this matrix3 added to the supplied matrix3.
         */
        matrix3 operator+(const matrix3 &m) const noexcept;

        /**
         * Get a reference to the element at the supplied index.
         *
         * @param index
         *   Index of element to get.
         *
         * @returns
         *   Reference to element at supplied index.
         */
        float& operator[](const std::size_t index) noexcept;

        /**
         * Get a copy of the element at the supplied index.
         *
         * @param index
         *   Index of element to get.
         *
         * @returns
         *   Copy of element at supplied index.
         */
        float operator[](const std::size_t index) const noexcept;

        /**
         * Get a pointer to the start of the internal matrix3 data array.
         *
         * @returns
         *   Pointer to start if matrix3 data.
         */
        const float* data() const noexcept;

        /**
         * Inverts this matrix.
         *
         * @returns
         *   A reference to this matrix3.
         */
        matrix3& invert() noexcept;

        /**
         * Transposes this matrix.
         *
         * @returns
         *   A reference to this matrix3.
         */
        matrix3& transpose() noexcept;

        /**
         * Static method to invert a matrix.
         *
         * @param m
         *   Matrix to invert.
         *
         * @returns
         *   A copy of the supplied matrix inverted.
         */
        static matrix3 invert(const matrix3 &m) noexcept;

        /**
         * Static method to transpose a matrix.
         *
         * @param m
         *   Matrix to transpose.
         *
         * @returns
         *   A copy of the supplied matrix transposed.
         */
        static matrix3 transpose(const matrix3 &m) noexcept;

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
        static matrix3 make_orthonormal_basis(const vector3 &x) noexcept;

        /**
         * Static method to make a skew-symmetric matrix, i.e. a matrix whose
         * transpose is equal to its negative. This takes a vector as a
         * skew-symmetric matrix can be used to compute a cross product.
         *
         * @param v
         *   vector3 from which values will be used so that a cross product
         *   can be calculated.
         *
         * @returns
         *   Skew-symmetric matrix.
         */
        static matrix3 make_skew_symmetric(const vector3 &v) noexcept;

        /**
         * Writes the matrix3 to the stream, useful for debugging.
         *
         * @param out
         *   The stream to write to.
         *
         * @param out
         *   The matrix3 to write to the stream.
         *
         * @return
         *   A reference to the supplied stream, after the matrix3 has been
         *   written.
         */
        friend std::ostream& operator<<(std::ostream &out, const matrix3 &m) noexcept;

    private:

        /** matrix3 data. */
        std::array<float, 9u> elements_;
};

}

