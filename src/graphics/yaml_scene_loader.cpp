////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/yaml_scene_loader.h"

#include <functional>
#include <memory>
#include <string_view>
#include <vector>

#include "core/quaternion.h"
#include "core/resource_manager.h"
#include "core/vector3.h"
#include "graphics/mesh_manager.h"
#include "graphics/scene.h"
#include "graphics/single_entity.h"

#include "yaml-cpp/yaml.h"

namespace YAML
{

template <>
struct convert<iris::Vector3>
{
    static Node encode(const iris::Vector3 &rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        return node;
    }

    static bool decode(const Node &node, iris::Vector3 &rhs)
    {
        if (!node.IsSequence() || node.size() != 3)
        {
            return false;
        }

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        return true;
    }
};

template <>
struct convert<iris::Quaternion>
{
    static Node encode(const iris::Quaternion &rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.push_back(rhs.w);
        return node;
    }

    static bool decode(const Node &node, iris::Quaternion &rhs)
    {
        if (!node.IsSequence() || node.size() != 4)
        {
            return false;
        }

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        rhs.w = node[3].as<float>();
        return true;
    }
};

}

namespace iris
{

struct YamlSceneLoader::implementation
{
    ::YAML::Node config;
};

YamlSceneLoader::YamlSceneLoader(Context &ctx, std::string_view file_name)
    : ctx_(ctx)
    , impl_(std::make_unique<implementation>())
{
    const auto file_contents = ctx_.resource_manager().load(file_name);
    std::string file_as_str(file_contents.size(), '\0');
    std::transform(
        std::cbegin(file_contents),
        std::cend(file_contents),
        std::begin(file_as_str),
        [](auto byte) { return static_cast<char>(byte); });

    impl_->config = ::YAML::Load(file_as_str);
}

YamlSceneLoader::~YamlSceneLoader() = default;

void YamlSceneLoader::load(
    Scene *scene,
    std::function<void(const std::vector<iris::SingleEntity *> &, std::string_view file_name)> entity_callback) const
{
    for (const auto &entity : impl_->config["models"])
    {
        const auto file_name = entity["file_name"].as<std::string>();
        const auto position = entity["position"].as<iris::Vector3>();
        const auto rotation = entity["rotation"].as<iris::Quaternion>();
        const auto scale = entity["scale"].as<iris::Vector3>();

        const auto loaded_mesh = ctx_.mesh_manager().load_mesh(file_name);

        std::vector<SingleEntity *> entities{};

        for (const auto &mesh : loaded_mesh.mesh_data)
        {
            entities.push_back(scene->create_entity<iris::SingleEntity>(
                nullptr, mesh.mesh, iris::Transform{position, rotation, scale}));
        }

        if (entity_callback)
        {
            entity_callback(entities, file_name);
        }
    }
}

}
