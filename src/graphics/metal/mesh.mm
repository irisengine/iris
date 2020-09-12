#include "graphics/mesh.h"

#include <any>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/vector3.h"
#include "graphics/buffer.h"
#include "graphics/buffer_type.h"
#include "graphics/vertex_data.h"

namespace iris
{

/**
 * Struct containing implementation specific data.
 */
struct Mesh::implementation
{
};

Mesh::Mesh(
    const std::vector<vertex_data> &vertices,
    const std::vector<std::uint32_t> &indices,
    Texture &&textures)
    : indices_(indices),
      texture_(std::move(textures)),
      vertex_buffer_(nullptr),
      index_buffer_(std::make_unique<Buffer>(indices, BufferType::DONT_CARE)),
      impl_(nullptr)
{
    // metal prefers four byte aligned data types, as vertex_data consists of
    // three byte components we 'extend' them by copying them into an enlarged
    // Buffer but skipping the fourth byte, this gives every object in
    // vertex_data a fourth component with the value one

    // before copying
    // +------------+------------+------------+------------+
    // |     1      |     1      |     1      |     1      |
    // +------------+------------+------------+------------+
    // |     1      |     1      |     1      |     1      |
    // +------------+------------+------------+------------+
    // |     1      |     1      |     1      |     1      |
    // +------------+------------+------------+------------+
    // |     1      |     1      |     1      |     1      |
    // +------------+------------+------------+------------+

    // after copying
    // +------------+------------+------------+------------+
    // | position.x | position.y | position.z |     1      |
    // +------------+------------+------------+------------+
    // |  normal.x  |  normal.y  |  normal.z  |     1      |
    // +------------+------------+------------+------------+
    // |  colour.x  |  colour.y  |  colour.z  |     1      |
    // +------------+------------+------------+------------+
    // | texcoord.x | texcoord.y | texcoord.z |     1      |
    // +------------+------------+------------+------------+

    // create a vector large enough to hold the data metal is expecting
    std::vector<float> data(vertices.size() * 16u, 1.0f);

    // get a pointer to the start of the supplied vertex data, we deliberately
    // treat it as a float array as this makes copying easier
    auto *src = reinterpret_cast<const float*>(vertices.data());

    // copy supplied data into larger buffer
    for(auto i = 0u; i < data.size(); ++i)
    {
        // skip every fourth copy as this will be the 'extension' byte and will
        // already have the value one
        if((i > 0u) && (((i + 1u) % 4u) == 0u))
        {
            continue;
        }

        data[i] = *src;
        ++src;
    }

    // create Buffer with four byte aligned data
    vertex_buffer_ = std::make_unique<Buffer>(data, BufferType::VERTEX_ATTRIBUTES);
}

/** Default */
Mesh::~Mesh() = default;
Mesh::Mesh(Mesh&&) = default;
Mesh& Mesh::operator=(Mesh&&) = default;

const std::vector<std::uint32_t>& Mesh::indices() const
{
    return indices_;
}

const Buffer& Mesh::vertex_buffer() const
{
    return *vertex_buffer_;
}

const Buffer& Mesh::index_buffer() const
{
    return *index_buffer_;
}

const Texture& Mesh::texture() const
{
    return texture_;
}

std::any Mesh::native_handle() const
{
    return { };
}

}

