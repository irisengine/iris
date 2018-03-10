#pragma once

#include "rigid_body.hpp"
#include "vector3.hpp"

namespace eng
{

/**
 * Implementation of rigid_body for a box shaped object.
 */
class box final : public rigid_body
{
    public:

        /**
         * Construct a new box.
         *
         * @param position
         *   Position of the centre of mass in world space.
         *
         * @param mass
         *   Mass of body.
         *
         * @param half_size
         *   The half size of the box.
         *
         * @param is_static
         *   Whether the rigid body is sttaic or not.
         */
        box(
            const vector3 &position,
            const float mass,
            const vector3 &half_size,
            const bool is_static);

        /** Default */
        ~box() override = default;
        box(const box&) = default;
        box& operator=(const box&) = default;
        box(box&&) = default;
        box& operator=(box&&) = default;

        /**
         * Get the half size of the box.
         *
         * @returns
         *   Box half size.
         */
        vector3 half_size() const noexcept;

    private:

        /** Half size of box. */
        vector3 half_size_;
};

}

