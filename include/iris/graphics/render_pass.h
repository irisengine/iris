////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/camera.h"
#include "graphics/cube_map.h"
#include "graphics/post_processing_description.h"
#include "graphics/render_target.h"
#include "graphics/scene.h"

namespace iris
{

/**
 * Struct encapsulating data needed for a render pass.
 * This describes:
 *  - what to render
 *  - where to render it from
 *  - where to render it to
 *
 * It is an engine convention that a nullptr target means render to the default target i.e. the window.
 */
struct RenderPass
{
    /** Scene to render */
    Scene *scene = nullptr;

    /** Camera to render with. */
    const Camera *camera = nullptr;

    /** Target to render to. */
    const RenderTarget *colour_target = nullptr;

    const RenderTarget *normal_target = nullptr;

    const RenderTarget *position_target = nullptr;

    /** Flag indicating that only depth information should be rendered. */
    bool depth_only = false;

    /** Optional sky box. */
    const CubeMap *sky_box = nullptr;

    /** Should the colour buffer be cleared before rendering. */
    bool clear_colour = true;

    /** Should the depth buffer be cleared before rendering. */
    bool clear_depth = true;

    PostProcessingDescription post_processing_description = PostProcessingDescription{};
};

}
