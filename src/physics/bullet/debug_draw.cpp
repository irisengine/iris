#include "physics/bullet/debug_draw.h"

#include <btBulletDynamicsCommon.h>

#include "core/exception.h"
#include "core/root.h"
#include "core/vector3.h"
#include "graphics/mesh_factory.h"
#include "graphics/model.h"

namespace iris
{

DebugDraw::DebugDraw()
    : verticies_()
    , model_(nullptr)
    , debug_mode_(0)
{
}

void DebugDraw::drawLine(
    const ::btVector3 &from,
    const ::btVector3 &to,
    const ::btVector3 &colour)
{
    verticies_.emplace_back(
        Vector3{from.x(), from.y(), from.z()},
        Vector3{colour.x(), colour.y(), colour.z()},
        Vector3{to.x(), to.y(), to.z()},
        Vector3{colour.x(), colour.y(), colour.z()});
}

void DebugDraw::render()
{
    if (!verticies_.empty())
    {
        auto &rs = Root::render_system();

        // remove old model, we build it fresh each frame
        if (model_ != nullptr)
        {
            rs.remove(model_);
        }

        // create mesh for all the debug lines
        model_ = rs.create<Model>(
            Vector3{}, Vector3{1.0f}, mesh_factory::lines(verticies_));
        model_->set_primitive_type(PrimitiveType::LINES);

        // clear data for next frame
        verticies_.clear();
    }
}
void DebugDraw::drawContactPoint(
    const ::btVector3 &,
    const ::btVector3 &,
    ::btScalar,
    int,
    const ::btVector3 &)
{
    throw Exception("unimplemented");
}

void DebugDraw::reportErrorWarning(const char *)
{
    throw Exception("unimplemented");
}

void DebugDraw::draw3dText(const ::btVector3 &, const char *)
{
    throw Exception("unimplemented");
}

void DebugDraw::setDebugMode(int debugMode)
{
    debug_mode_ = debugMode;
}

int DebugDraw::getDebugMode() const
{
    return debug_mode_;
}

}
