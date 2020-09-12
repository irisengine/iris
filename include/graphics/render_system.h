#pragma once

#include <memory>
#include <type_traits>
#include <vector>

#include "core/camera.h"
#include "core/camera_type.h"
#include "core/vector3.h"
#include "graphics/font.h"
#include "graphics/material.h"
#include "graphics/render_entity.h"

namespace iris
{

/**
 * Class for rendering scenes.
 */
class RenderSystem
{
    public:

        // helper trait
        template<class T>
        using is_render_entity = std::enable_if_t<std::is_base_of_v<RenderEntity, T>>;

        /**
         * Create a new rendering system.
         *
         * @param width
         *   Width of window.
         *
         * @param height
         *   Height of window.
         */
        RenderSystem(float width, float height);

        /** Declared in mm/cpp file as implementation is an incomplete file. */
        ~RenderSystem();
        RenderSystem(RenderSystem&&);
        RenderSystem& operator=(RenderSystem&&);

        /**
         * Create a RenderEntity and add it to the scene. Uses perfect
         * forwarding to pass along all arguments.
         *
         * @param args
         *   Arguments for RenderEntity.
         *
         * @returns
         *    A pointer to the newly created RenderEntity.
         */
        template<class T, class ...Args, typename=is_render_entity<T>>
        RenderEntity* create(Args &&...args)
        {
            auto element = std::make_unique<T>(std::forward<Args>(args)...);
            
            return add(std::move(element));
        }

        /**
         * Add a RenderEntity to the scene.
         *
         * @param entity
         *   RenderEntity to render.
         *
         * @returns
         *   Pointer to the added RenderEntity.
         */
        RenderEntity* add(std::unique_ptr<RenderEntity> entity);

        /**
         * Render the current scene.
         */
        void render();

        /**
         * Get the perspective camera.
         *
         * @returns
         *  Perspective camera.
         */
        Camera& persective_camera();

        /**
         * Get the orthographic camera.
         *
         * @returns
         *  Orthographic camera.
         */
        Camera& orthographic_camera();

        /**
         * Get the camera for the given type.
         *
         * @param type
         *   Type of camera to get.
         *
         * @returns
         *   Camera of requested type.
         */
        Camera& camera(CameraType type);

    private:

        /** Collection of entities in a scene to render. */
        std::vector<std::unique_ptr<RenderEntity>> scene_;

        /** Perspective camera. */
        Camera persective_camera_;

        /** Orthographic camera. */
        Camera orthographic_camera_;

        /** Pointer to implementation. */
        struct implementation;
        std::unique_ptr<implementation> impl_;
};

}

