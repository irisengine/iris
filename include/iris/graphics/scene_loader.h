////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <functional>
#include <vector>

namespace iris
{

class Scene;
class SingleEntity;

class SceneLoader
{
  public:
    virtual ~SceneLoader() = default;

    virtual void load(
        Scene *scene,
        std::function<void(const std::vector<iris::SingleEntity *> &, std::string_view file_name)> entity_callback =
            nullptr) const = 0;

  private:
};

}
