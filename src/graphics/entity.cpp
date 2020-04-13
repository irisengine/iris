#include "entity.hpp"

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "exception.hpp"
#include "log.hpp"
#include "material.hpp"
#include "material_factory.hpp"
#include "matrix4.hpp"
#include "quaternion.hpp"
#include "vector3.hpp"

namespace
{

/**
 * Helper function that reads a file into a std::string.
 *
 * @param path
 *   Path of file to read.
 *
 * @returns
 *   The contents of the file as a std::string.
 */
std::string read_file(const std::filesystem::path &path)
{
    if(!std::filesystem::exists(path))
    {
        throw eng::exception(path.string() + " does not exist");
    }

    std::ifstream in{ path };
    std::stringstream strm{ };
    strm << in.rdbuf();

    return strm.str();
}

/**
 * Helper function to split a string by delimiter.
 *
 * @param input
 *   String to split.
 *
 * @param delim
 *   Delimiter to split at.
 *
 * @returns
 *   A collection if strings, split from the input.
 */
std::vector<std::string> split(const std::string &input, const char delim)
{
    std::vector<std::string> parts{ };

    std::stringstream strm{ input };
    std::string part{ };

    while(static_cast<bool>(std::getline(strm, part, delim)) != false)
    {
        parts.emplace_back(part);
    }

    return parts;
}

/**
 * Helper function to load a wavefront material file.
 *
 * @param parg
 *   Path of file to load.
 *
 * @returns
 *   A map of material names to diffuse colour and image files.
 */
std::map<std::string, std::tuple<eng::vector3, std::filesystem::path>> load_material(
    const std::filesystem::path &path)
{
    const auto material_file = read_file(path);

    std::map<std::string, std::tuple<eng::vector3, std::filesystem::path>> materials{ };
    eng::vector3 current_diffuse{ };
    std::filesystem::path current_texture{ };
    std::string current_name{ };

    const auto lines = split(material_file, '\n');

    // parse each line
    for(const auto &line : lines)
    {
        // ignore empty lines and comments
        if(line.empty() || (line.front() == '#'))
        {
            continue;
        }

        // find the first space in the file, if one doesn't exist then ignore
        const auto space_pos = line.find(' ');
        if(space_pos == std::string::npos)
        {
            continue;
        }

        // using the location of the first space separate the line into
        // an element and tag and args
        const auto element = line.substr(0u, space_pos);
        const auto args = line.substr(space_pos + 1u);

        if(element == "newmtl")
        {
            // process new material

            if(!current_name.empty())
            {
                // we are already processing a material, so save that off
                materials[current_name] = std::make_tuple(current_diffuse, current_texture);
            }

            // set name of current material
            current_name = args;
        }
        else if(element == "Kd")
        {
            // process diffuse colour

            auto r = 0.0f;
            auto g = 0.0f;
            auto b = 0.0f;

            // extract colour values
            if(std::sscanf(args.c_str(), "%f %f %f", &r, &g, &b) != 3u)
            {
                throw eng::exception("only support colour in format: v r g b");
            }

            current_diffuse = { r, g, b };
        }
        else if(element == "map_Kd")
        {
            // process diffuse image

            current_texture = std::filesystem::path(path).replace_filename(args);
        }
    }

    // save off the last material we were processing
    materials[current_name] = std::make_tuple(current_diffuse, current_texture);

    return materials;
}

/**
 * Helper function to parse a 3d model file and create an internal engine
 * representation from it.
 *
 * This supports a subset of the wavefront object file format.
 *
 * @param path
 *   Path to model file to load.
 *
 * @returns
 *   Collection of parsed meshes.
 */
std::vector<eng::mesh> load_file(const std::filesystem::path &path)
{
    std::vector<eng::mesh> meshes{ };

    const auto file_data = read_file(path);
    const auto lines = split(file_data, '\n');

    std::map<std::string, std::tuple<eng::vector3, std::filesystem::path>> materials{ };
    std::vector<eng::vector3> vertices{ };
    std::vector<eng::vector3> normals{ };
    std::vector<eng::vector3> texture_coords{ };
    std::vector<std::uint32_t> indices{ };
    std::vector<eng::vertex_data> vertex_data{ };
    std::map<std::string, std::uint32_t> vertex_data_map{ };
    std::string current_object{ };
    std::string current_material{ };

    // process each line
    for(const auto &line : lines)
    {
        // ignore empty lines and comments
        if(line.empty() || (line.front() == '#'))
        {
            continue;
        }

        // find the first space in the file, if one doesn't exist then ignore
        const auto space_pos = line.find(' ');
        if(space_pos == std::string::npos)
        {
            continue;
        }

        // using the location of the first space separate the line into
        // an element and tag and args
        const auto element = line.substr(0u, space_pos);
        const auto args = line.substr(space_pos + 1u);


        if(element == "mtllib")
        {
            // load material file
            const auto material_filename = args;

            const auto material = std::filesystem::path(path).replace_filename(material_filename);

            materials = load_material(material);
        }
        else if(element == "usemtl")
        {
            // set current material

            current_material = args;
        }
        else if(element == "o")
        {
            if(!current_object.empty())
            {
                // we are already processing an object, so save that off

                //create texture from file if it exists, else default to an all white image
                const auto tex_path = std::get<1>(materials.at(current_material));
                auto texture = std::filesystem::exists(tex_path)
                    ? eng::texture{ tex_path }
                    : eng::texture{ { 0xFF, 0xFF, 0xFF }, 1u, 1u, 3u };

                meshes.emplace_back(vertex_data, indices, std::move(texture));

                // clear stored data for next object
                vertex_data = { };
                indices = { };
            }

            // set current object name
            current_object = args;
        }
        else if(element == "v")
        {
            // process vertex data

            auto x = 0.0f;
            auto y = 0.0f;
            auto z = 0.0f;

            // extract vertex data
            if(std::sscanf(args.c_str(), "%f %f %f", &x, &y, &z) != 3u)
            {
                throw eng::exception("only support vertex in format: v x y z");
            }

            vertices.emplace_back(x, y, z);
        }
        else if(element == "vn")
        {
            // process normal data

            auto x = 0.0f;
            auto y = 0.0f;
            auto z = 0.0f;

            // extract normal data
            if(std::sscanf(args.c_str(), "%f %f %f", &x, &y, &z) != 3u)
            {
                throw eng::exception("only support normals in format: vn x y z");
            }

            normals.emplace_back(x, y, z);
            normals.back().normalise();
        }
        else if(element == "vt")
        {
            // process texture coordinate data

            auto x = 0.0f;
            auto y = 0.0f;

            // extract texture coordinate data
            if(std::sscanf(args.c_str(), "%f %f", &x, &y) != 2u)
            {
                throw eng::exception("only support tex coords in format: vt x y");
            }

            texture_coords.emplace_back(x, y, 0.0f);
        }
        else if(element.substr(0, 1) == "f")
        {
            // process face data

            // args are in the form: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
            // so split on the space to get the separate prts
            const auto arg_parts = split(args, ' ');

            // process each part
            for(const auto &arg : arg_parts)
            {
                // split again on the slash
                const auto parts = split(arg, '/');

                // only support subset of the wavefront format
                if(parts.size() != 3u)
                {
                    throw eng::exception("only support faces with format: f v1/vt1/vn1");
                }

                // check if we have processed data for this vertex before
                const auto index_iter = vertex_data_map.find(arg);

                if(index_iter != std::cend(vertex_data_map))
                {
                    // if we have then just use the existing index
                    indices.emplace_back((*index_iter).second);
                }
                else
                {
                    // this is a new vertex, so process

                    // get tex coord, maybe empty so default to empty vector
                    const auto tex_coord = parts[1].empty()
                        ? eng::vector3{ }
                        : texture_coords[std::stoi(parts[1]) - 1u];

                    // check and get vertex position data
                    const auto vertex_index = std::stoi(parts[0u]) - 1u;
                    if(vertex_index > vertices.size())
                    {
                        throw eng::exception("missing vertex");
                    }

                    // check and get vertex normal data
                    const auto normal_index = std::stoi(parts[2u]) - 1u;
                    if(normal_index > normals.size())
                    {
                        throw eng::exception("missing normal");
                    }

                    // create vertex
                    vertex_data.emplace_back(
                        vertices[vertex_index],
                        normals[normal_index],
                        std::get<0>(materials.at(current_material)),
                        tex_coord);

                    // add this data to map so we can gets its index if we
                    // see it again
                    vertex_data_map[arg] = vertex_data.size() - 1u;
                    indices.emplace_back(vertex_data.size() - 1u);
                }
            }
        }
    }

    // save of the last mesh we were processing

    //create texture from file if it exists, else default to an all white image
    const auto tex_path = std::get<1>(materials.at(current_material));
    auto texture = std::filesystem::exists(tex_path)
        ? eng::texture{ tex_path }
        : eng::texture{ { 0xFF, 0xFF, 0xFF }, 1u, 1u, 3u };
    meshes.emplace_back(vertex_data, indices, std::move(texture));

    return meshes;
}

}
namespace eng
{

entity::entity(
    const std::filesystem::path &path,
    const vector3 &position,
    const quaternion &orientation,
    const vector3 &scale)
    : entity(load_file(path), position, orientation, scale)
{ }

entity::entity(
    const std::filesystem::path &path,
    const vector3 &position,
    const quaternion &orientation,
    const vector3 &scale,
    std::shared_ptr<material> mat)
    : entity(load_file(path), position, orientation, scale, mat)
{ }

entity::entity(
    std::vector<mesh> &&meshes,
    const vector3 &position,
    const quaternion &orientation,
    const vector3 &scale)
    : entity(std::move(meshes), position, orientation, scale, material_factory::basic_mesh())
{ }

entity::entity(
    std::vector<mesh> &&meshes,
    const vector3 &position,
    const quaternion &orientation,
    const vector3 &scale,
    std::shared_ptr<material> mat)
    : meshes_(std::move(meshes)),
      position_(position),
      orientation_(orientation),
      scale_(scale),
      model_(matrix4::make_translate(position) * matrix4(orientation) * matrix4::make_scale(scale)),
      material_(mat),
      wireframe_(false)
{
    LOG_INFO("entity", "constructed at: {}", position_);
}

void entity::set_position(const vector3 &position)
{
    position_ = position;

    model_ = matrix4::make_translate(position_) * matrix4(orientation_) * matrix4::make_scale(scale_);
}

void entity::set_orientation(const quaternion &orientation)
{
    orientation_ = orientation;
    model_ = matrix4::make_translate(position_) * matrix4(orientation_) * matrix4::make_scale(scale_);
}

matrix4 entity::transform() const
{
    return model_;
}

const std::vector<mesh>& entity::meshes() const
{
    return meshes_;
}

const material& entity::mat() const
{
    return *material_;
}

bool entity::should_render_wireframe() const
{
    return wireframe_;
}

void entity::set_wireframe(const bool wireframe)
{
    wireframe_ = wireframe;
}

}

