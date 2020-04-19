#include "graphics/shape_factory.h"

#include <cstdint>
#include <vector>

#include "core/vector3.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/vertex_data.h"

namespace eng::shape_factory
{

Mesh sprite(
    const Vector3 &colour,
    Texture &&tex)
{
    std::vector<vertex_data> vertices {
        { { -0.5f,  0.5f, 0.0f }, { } , colour, { 0.0f, 1.0f, 0.0f } },
        { {  0.5f,  0.5f, 0.0f }, { } , colour, { 1.0f, 1.0f, 0.0f } },
        { {  0.5f, -0.5f, 0.0f }, { } , colour, { 1.0f, 0.0f, 0.0f } },
        { { -0.5f, -0.5f, 0.0f }, { } , colour, { 0.0f, 0.0f, 0.0f } }
    };

    std::vector<std::uint32_t> indices {
        0, 2, 1, 3, 2, 0
    };

    return {
        vertices,
        indices,
        std::move(tex)
    };
}

}

