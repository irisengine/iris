#pragma once

#include "core/colour.h"
#include "graphics/lights/directional_light.h"
#include "graphics/lights/point_light.h"
#include "graphics/lights/ambient_light.h"

#include <memory>
#include <vector>

namespace iris
{

/**
 * Encapsulates all the lights for a scene.
 */
struct LightingRig
{
    /* Collection of point lights. */
    std::vector<std::unique_ptr<PointLight>> point_lights;

    /** Collection of directional lights. */
    std::vector<std::unique_ptr<DirectionalLight>> directional_lights;

    /** Ambient light colour. */
    std::unique_ptr<AmbientLight> ambient_light;
};

}
