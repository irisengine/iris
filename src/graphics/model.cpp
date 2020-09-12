#include "graphics/model.h"

#include "core/vector3.h"
#include "graphics/material_factory.h"
#include "graphics/mesh.h"
#include "graphics/render_entity.h"

namespace iris
{
Model::Model(const Vector3 &position, const Vector3 &scale, Mesh mesh)
    : RenderEntity(
        std::move(mesh),
        position,
        { },
        scale,
        material_factory::mesh(),
        false,
        CameraType::PERSPECTIVE)
{ }

}

