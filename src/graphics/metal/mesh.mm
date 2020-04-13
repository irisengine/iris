#include "mesh.hpp"

#include <any>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "buffer.hpp"
#include "buffer_type.hpp"
#include "matrix4.hpp"
#include "quaternion.hpp"
#include "vector3.hpp"
#include "vertex_data.hpp"

namespace eng
{

/**
 * Struct containing implementation specific data.
 */
struct mesh::implementation
{
};

mesh::mesh(
    const std::vector<vertex_data> &vertices,
    const std::vector<std::uint32_t> &indices,
    texture &&textures)
    : indices_(indices),
      texture_(std::move(textures)),
      vertex_buffer_(nullptr),
      index_buffer_(std::make_unique<buffer>(indices, buffer_type::DONT_CARE)),
      impl_(nullptr)
{
    // metal prefers four byte aligned data types, as vertex_data consists of
    // three byte components we 'extend' them by copying them into an enlarged
    // buffer but skipping the fourth byte, this gives every object in
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

    // create buffer with four byte aligned data
    vertex_buffer_ = std::make_unique<buffer>(data, buffer_type::VERTEX_ATTRIBUTES);
}

/** Default */
mesh::~mesh() = default;

const std::vector<std::uint32_t>& mesh::indices() const
{
    return indices_;
}

const buffer& mesh::vertex_buffer() const
{
    return *vertex_buffer_;
}

const buffer& mesh::index_buffer() const
{
    return *index_buffer_;
}

const texture& mesh::tex() const
{
    return texture_;
}

std::any mesh::native_handle() const
{
    return { };
}

}

