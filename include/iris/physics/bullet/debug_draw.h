#pragma once

#include <vector>

#include <LinearMath/btIDebugDraw.h>
#include <LinearMath/btVector3.h>

#include "core/vector3.h"
#include "graphics/render_entity.h"

namespace iris
{

/**
 * Implementation of bullets btIDebugDraw. This class enables bullet debug
 * information to be rendered with the engine.
 *
 * The btIDebugDraw was designed for immediate mode rendering and so the methods
 * will be continuously called with a series of elements to render. In order to
 * fit in with the design of this engine all these elements are buffered. A
 * render() method is exposed to process all the data sent by bullet and pass
 * onto the rendering part of the engine.
 */
class DebugDraw : public ::btIDebugDraw
{
  public:
    /**
     * Construct a new DebugDraw.
     */
    DebugDraw(RenderEntity *entity);

    // default
    ~DebugDraw() override = default;

    // the following methods are part of the bullet interface, see bullet
    // documentation for details
    // note they are not all implemented

    void drawLine(
        const ::btVector3 &from,
        const ::btVector3 &to,
        const ::btVector3 &colour) override;

    void drawContactPoint(
        const ::btVector3 &PointOnB,
        const ::btVector3 &normalOnB,
        ::btScalar distance,
        int lifeTime,
        const ::btVector3 &color) override;

    void reportErrorWarning(const char *warningString) override;

    void draw3dText(const ::btVector3 &location, const char *textString)
        override;

    void setDebugMode(int debugMode) override;

    int getDebugMode() const override;

    /**
     * Render all the elements from the above calls.
     */
    void render();

  private:
    /** Collection of vertices to render. */
    std::vector<std::tuple<Vector3, Vector3, Vector3, Vector3>> verticies_;

    /** A render entity for all debug shapes. */
    RenderEntity *entity_;

    /** Bullet specific debug mode. */
    int debug_mode_;
};

}
