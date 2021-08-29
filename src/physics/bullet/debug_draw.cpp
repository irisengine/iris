#include "physics/bullet/debug_draw.h"

#include <cstdint>
#include <vector>

#include <btBulletDynamicsCommon.h>

#include "core/colour.h"
#include "core/exception.h"
#include "core/root.h"
#include "core/vector3.h"
#include "graphics/mesh.h"
#include "graphics/vertex_data.h"

namespace iris
{

DebugDraw::DebugDraw(RenderEntity *entity)
    : verticies_()
    , entity_(entity)
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
        Colour{colour.x(), colour.y(), colour.z()},
        Vector3{to.x(), to.y(), to.z()},
        Colour{colour.x(), colour.y(), colour.z()});
}

void DebugDraw::render()
{
    if (!verticies_.empty())
    {
        std::vector<VertexData> vertices{};
        std::vector<std::uint32_t> indices;

        for (const auto &[from_position, from_colour, to_position, to_colour] :
             verticies_)
        {
            vertices.emplace_back(
                from_position, Vector3{1.0f}, from_colour, Vector3{});
            indices.emplace_back(
                static_cast<std::uint32_t>(vertices.size() - 1u));

            vertices.emplace_back(
                to_position, Vector3{1.0f}, to_colour, Vector3{});
            indices.emplace_back(
                static_cast<std::uint32_t>(vertices.size() - 1u));
        }

        entity_->mesh()->update_vertex_data(vertices);
        entity_->mesh()->update_index_data(indices);

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
