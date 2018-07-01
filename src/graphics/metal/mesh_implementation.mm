#include "metal/mesh_implementation.hpp"

#include <cstring>
#include <utility>
#include <vector>

#include "vector3.hpp"
#include "vertex_data.hpp"

namespace eng
{

mesh_implementation::mesh_implementation(
    const std::vector<vertex_data> &vertices,
    const std::vector<std::uint32_t> &indices)
    : vertex_buffer_(nullptr),
      index_buffer_(std::make_unique<buffer>(indices))
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
    vertex_buffer_ = std::make_unique<buffer>(data);
}

const buffer& mesh_implementation::vertex_buffer() const noexcept
{
    return *vertex_buffer_;
}

const buffer& mesh_implementation::index_buffer() const noexcept
{
    return *index_buffer_;
}

std::any mesh_implementation::native_handle() const
{
    return { };
}

}


