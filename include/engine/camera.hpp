#pragma once

#include "vector3.hpp"

namespace eng
{

/**
 * Class representing the camera through which a scene is rendered.
 */
class camera final
{
    public:

        /** Default */
        camera() = default;
        ~camera() = default;
        camera(const camera&) = default;
        camera& operator=(const camera&) = default;
        camera(camera&&) = default;
        camera& operator=(camera&&) = default;

        /**
         * Get position of camera.
         *
         * @returns
         *   Camera position in world space.
         */
        vector3 position() const noexcept;

   private:

        /** Camera position in world space. */
        vector3 position_;
};

}

