#pragma once

#include "core/camera.h"
#include "core/vector3.h"
#include "graphics/render_target.h"

namespace iris
{

/**
 * A directional light. This is a light infinitely far away from the scene and
 * consistent in all directions.
 *
 * A light may be constructed to cast shadows, this will cause extra render
 * passes to be created which can impact performance (depending in scene
 * complexity).
 */
class DirectionalLight
{
  public:
    /** Create a new DirectionalLight.
     *
     * @param direction
     *   The direction the rays of light are pointing, for examples to have a
     *   light shining directly down on a scene then its direction would be
     *   (0, -1, 0).
     *
     * @param cast_shadows
     *   True if this light should generate shadows, false otherwise.
     */
    DirectionalLight(const Vector3 &direction, bool cast_shadows = false);

    /**
     * Get direction of light.
     *
     * @returns
     *   Light direction.
     */
    Vector3 direction() const;

    /**
     * Set direction of light.
     *
     * @param direction
     *   New light direction.
     */
    void set_direction(const Vector3 &direction);

    /**
     * Check if this light should cast shadows.
     *
     * @returns
     *   True if light casts shadows, false otherwise.
     */
    bool casts_shadows() const;

    /**
     * Get the camera used for rendering the shadow map for this light.
     *
     * This is used internally and should not normally be called manually.
     *
     * @returns
     *   Shadow map camera.
     */
    Camera &shadow_camera();

    /**
     * Get the render target for this lights shadow map.
     *
     * This is used internally and should not normally be called manually.
     *
     * @returns
     *   Shadow map render target.
     */
    RenderTarget *shadow_target();

  private:
    /** Light direction. */
    Vector3 direction_;

    /** Shadow map camera. */
    Camera shadow_camera_;

    /** Shadow map render target. */
    RenderTarget shadow_target_;

    /** Should shadows be generated. */
    bool cast_shadows_;
};

}
