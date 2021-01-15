#include "physics/bullet/debug_draw.h"

#include <btBulletDynamicsCommon.h>

#include "core/colour.h"
#include "core/exception.h"
#include "core/vector3.h"
#include "graphics/mesh_factory.h"
#include "graphics/render_system.h"

namespace iris
{

DebugDraw::DebugDraw(RenderEntity *entity)
    : verticies_()
    , entity_(entity)
    , debug_mode_(0)
{
    entity_->set_primitive_type(PrimitiveType::LINES);
}

void DebugDraw::drawLine(
    const ::btVector3 &from,
    const ::btVector3 &to,
    const ::btVector3 &colour)
{
    verticies_.emplace_back(
        Vector3{from.x(), from.y(), from.z()},
        Colour{colour.x(), colour.y(), colour.z()},
        Vector3{to.x(), to.y(), to.z()},
        Colour{colour.x(), colour.y(), colour.z()});
}

void DebugDraw::render()
{
    if (!verticies_.empty())
    {
        auto mesh = mesh_factory::lines(verticies_);
        entity_->set_mesh(std::move(mesh));

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
