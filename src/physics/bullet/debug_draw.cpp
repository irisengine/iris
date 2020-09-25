#include "physics/bullet/debug_draw.h"

#include <btBulletDynamicsCommon.h>

#include "core/exception.h"
#include "core/root.h"
#include "core/vector3.h"
#include "graphics/model.h"

namespace iris
{

DebugDraw::DebugDraw()
    : verticies_()
    , indicies_()
    , model_(nullptr)
    , debug_mode_(0)
{
}

void DebugDraw::drawLine(
    const ::btVector3 &from,
    const ::btVector3 &to,
    const ::btVector3 &colour)
{
    // cache each line as a vertex and an index

    verticies_.emplace_back(
        Vector3{from.x(), from.y(), from.z()},
        Vector3{1.0f},
        Vector3{colour.x(), colour.y(), colour.z()},
        Vector3{});

    indicies_.emplace_back(verticies_.size() - 1u);

    verticies_.emplace_back(
        Vector3{to.x(), to.y(), to.z()},
        Vector3{1.0f},
        Vector3{colour.x(), colour.y(), colour.z()},
        Vector3{});

    indicies_.emplace_back(verticies_.size() - 1u);
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

        std::vector<Mesh> meshes{};

        BufferDescriptor descriptor(
            Buffer(verticies_, BufferType::VERTEX_ATTRIBUTES),
            Buffer(indicies_, BufferType::VERTEX_INDICES),
            vertex_attributes);

        meshes.emplace_back(std::move(descriptor));

        // create mesh for all the debug lines
        model_ = rs.create<Model>(Vector3{}, Vector3{1.0f}, std::move(meshes));
        model_->set_primitive_type(PrimitiveType::LINES);

        // clear data for next frame
        verticies_.clear();
        indicies_.clear();
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
