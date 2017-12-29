#pragma once

#include <array>
#include <iostream>

#include "vector3.hpp"

namespace eng
{

/**
 * Class represents a 4x4 matrix.
 *
 * Elements are stored in row-major order.
 */
class matrix final
{
    public:

        /**
         * Constructs a new identity matrix.
         */
        matrix();

        /** Default */
        matrix(const matrix&) = default;
        matrix& operator=(const matrix&) = default;
        matrix(matrix&&) = default;
        matrix& operator=(matrix&&) = default;

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
        static matrix make_projection(
            const float fov,
            const float aspect_ratio,
            const float near,
            const float far) noexcept;

        /**
         * Make a matrix that can be used as a view matrix for a camera.
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
         *   A matrix that can be used as a camera view matrix.
         */
        static matrix make_look_at(
            const vector3 &eye,
            const vector3 &look_at,
            const vector3 &up) noexcept;

        /**
         * Static method to create a scale matrix.
         *
         * @param scale
         *   Vector3 specifying amount to scale along each axis.
         *
         * @returns
         *   Scale transformation matrix.
         */
        static matrix make_scale(const vector3 &scale) noexcept;

        /**
         * Static method to create translation matrix.
         *
         * @param translate
         *   Vector to translate by.
         */
        static matrix make_translate(const vector3 &translate) noexcept;

        /**
         * Performs matrix multiplication.
         *
         * @param m
         *   The matrix to multiply.
         *
         * @returns
         *   This matrix multiplied the supplied matrix.
         */
        matrix& operator*=(const matrix &m) noexcept;

        /**
         * Performs matrix multiplication.
         *
         * @param m
         *   The matrix to multiply.
         *
         * @returns
         *   New matrix which is this matrix multiplied the supplied matrix.
         */
        matrix operator*(const matrix &m) const noexcept;

        /**
         * Get a reference to the element at the supplied index.
         *
         * @param index
         *   Index of element to get.
         *
         * @returns
         *   Reference to element at supplied index.
         */
        float& operator[](const size_t index) noexcept;

        /**
         * Get a copy of the element at the supplied index.
         *
         * @param index
         *   Index of element to get.
         *
         * @returns
         *   Copy of element at supplied index.
         */
        float operator[](const size_t index) const noexcept;

        /**
         * Get a pointer to the start of the internal matrix data array.
         *
         * @returns
         *   Pointer to start if matrix data.
         */
        const float* data() const noexcept;

        /**
         * Writes the matrix to the stream, useful for debugging.
         *
         * @param out
         *   The stream to write to.
         *
         * @param out
         *   The matrix to write to the stream.
         *
         * @return
         *   A reference to the supplied stream, after the matrix has been
         *   written.
         */
        friend std::ostream& operator<<( std::ostream &out, const matrix &m) noexcept;

    private:

        /** Matrix data */
        std::array<float, 16u> elements_;
};

}

