////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <functional>
#include <memory>
#include <string_view>
#include <vector>

#include "core/context.h"
#include "graphics/scene_loader.h"

namespace iris
{

class Scene;
class SingleEntity;

class YamlSceneLoader : public SceneLoader
{
  public:
    YamlSceneLoader(Context &ctx, std::string_view file_name);
    ~YamlSceneLoader() override;

    void load(
        Scene *scene,
        std::function<void(const std::vector<iris::SingleEntity *> &, std::string_view file_name)> entity_callback =
            nullptr) const override;

  private:
    Context &ctx_;
    struct implementation;
    std::unique_ptr<implementation> impl_;
};
}
