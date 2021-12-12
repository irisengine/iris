////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/camera.h"
#include "graphics/cube_map.h"
#include "graphics/render_target.h"
#include "graphics/scene.h"

namespace iris
{

/**
 * Struct encapsulating the high-level engin objects needed for a render pass.
 * This describes:
 *  - what to render
 *  - where to render it from
 *  - where to render it to
 *
 * It is an engine convention that a nullptr target means render to the default
 * target i.e. the window.
 */
struct RenderPass
{
    /**
     * Create a new RenderPass.
     *
     * @param scene
     *   The scene to render.
     *
     * @param camera
     *   The camera to render from
     *
     * @param target
     *   The target to render to, nullptr means the default window target.c
     *
     * @param sky_box
     *   Optional sky box to render scene with.
     */
    RenderPass(Scene *scene, Camera *camera, RenderTarget *target, CubeMap *sky_box = nullptr)
        : scene(scene)
        , camera(camera)
        , render_target(target)
        , depth_only(false)
        , sky_box(sky_box)
    {
    }

    /** Scene to render */
    Scene *scene;

    /** Camera to render with. */
    const Camera *camera;

    /** Target to render to. */
    const RenderTarget *render_target;

    /** Flag indicating that only depth information should be rendered. */
    bool depth_only = false;

    /** Optional sky box. */
    const CubeMap *sky_box;
};

}
