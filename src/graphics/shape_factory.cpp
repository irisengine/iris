#include "graphics/shape_factory.h"

#include <cstdint>
#include <vector>

#include "core/vector3.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/vertex_data.h"

namespace eng::shape_factory
{

Mesh sprite(const Vector3 &colour, Texture &&texture)
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

    return { vertices, indices, std::move(texture) };
}

Mesh cube(const Vector3 colour)
{
	std::vector<vertex_data> vertices {
        { { 1.0f, -1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } , colour, { } },
        { { -1.0f, -1.0f, -1.0f }, { 0.0f, -1.0f, 0.0f } , colour, { } },
        { { 1.0f, -1.0f, -1.0f }, { 0.0f, -1.0f, 0.0f } , colour, { } },
        { { -1.0f, 1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f } , colour, { } },
        { { 0.999999f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } , colour, { } },
        { { 1.0f, 1.0f, -0.999999f }, { 0.0f, 1.0f, 0.0f } , colour, { } },
        { { 1.0f, 1.0f, -0.999999f }, { 1.0f, -0.0f, 0.0f } , colour, { } },
        { { 1.0f, -1.0f, 1.0f }, { 1.0f, -0.0f, 0.0f } , colour, { } },
        { { 1.0f, -1.0f, -1.0f }, { 1.0f, -0.0f, 0.0f } , colour, { } },
        { { 0.999999f, 1.0f, 1.0f }, { 0.0f, -0.0f, 1.0f } , colour, { } },
        { { -1.0f, -1.0f, 1.0f }, { 0.0f, -0.0f, 1.0f } , colour, { } },
        { { 1.0f, -1.0f, 1.0f }, { 0.0f, -0.0f, 1.0f } , colour, { } },
        { { -1.0f, -1.0f, 1.0f }, { -1.0f, -0.0f, -0.0f } , colour, { } },
        { { -1.0f, 1.0f, -1.0f }, { -1.0f, -0.0f, -0.0f } , colour, { } },
        { { -1.0f, -1.0f, -1.0f }, { -1.0f, -0.0f, -0.0f } , colour, { } },
        { { 1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, -1.0f } , colour, { } },
        { { -1.0f, 1.0f, -1.0f }, { 0.0f, 0.0f, -1.0f } , colour, { } },
        { { 1.0f, 1.0f, -0.999999f }, { 0.0f, 0.0f, -1.0f } , colour, { } },
        { { -1.0f, -1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } , colour, { } },
        { { -1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } , colour, { } },
        { { 0.999999f, 1.0f, 1.0f }, { 1.0f, -0.0f, 0.0f } , colour, { } },
        { { -1.0f, 1.0f, 1.0f }, { 0.0f, -0.0f, 1.0f } , colour, { } },
        { { -1.0f, 1.0f, 1.0f }, { -1.0f, -0.0f, -0.0f } , colour, { } },
        { { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, -1.0f } , colour, { } }
    };

    std::vector<std::uint32_t> indices {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 0, 18, 1,
        3, 19, 4, 6, 20, 7, 9, 21, 10, 12, 22, 13, 15, 23, 16
    };

	return { vertices, indices, Texture::blank() };
}

}

