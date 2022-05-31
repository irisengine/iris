////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/animation/animation_controller.h"

#include <chrono>
#include <cstddef>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "core/error_handling.h"
#include "core/transform.h"
#include "graphics/animation/animation.h"
#include "graphics/animation/animation_layer.h"
#include "graphics/animation/animation_state.h"
#include "graphics/animation/animation_transition.h"
#include "graphics/animation/cached_bone_query.h"
#include "graphics/skeleton.h"
#include "log/log.h"

namespace
{

iris::AnimationState *get_state(std::string_view name, const std::vector<std::unique_ptr<iris::AnimationState>> &states)
{
    auto state = std::find_if(std::begin(states), std::end(states), [name](const auto &element) {
        return element->animation()->name() == name;
    });

    iris::expect(state != std::end(states), "missing state");

    return state->get();
}

}

namespace iris
{

AnimationController::AnimationController(
    const std::vector<Animation> &animations,
    const std::vector<AnimationLayer> &layers,
    Skeleton *skeleton)
    : skeleton_(skeleton)
    , animations_(animations)
    , layers_(layers)
    , states_(layers_.size())
    , current_state_(layers_.size())
    , query_()
{
    ensure(!layers.empty(), "must have at least one layer");
    ensure(layers.front().bone_mask.empty(), "base layer cannot have a mask");

    std::vector<std::set<std::string>> bone_masks{};

    for (auto i = 1u; i < layers.size(); ++i)
    {
        bone_masks.emplace_back(std::cbegin(layers[i].bone_mask), std::cend(layers[i].bone_mask));
    }

    query_ = std::make_unique<CachedBoneQuery>(skeleton_, bone_masks);

    for (auto i = 0u; i < layers_.size(); ++i)
    {
        const auto &layer = layers_[i];
        current_state_[i] = nullptr;

        for (auto &animation : animations_)
        {
            states_[i].emplace_back(
                std::make_unique<AnimationState>(std::addressof(animation), skeleton_, query_.get(), i));

            if (animation.name() == layer.start_animation)
            {
                current_state_[i] = states_[i].back().get();
            }
        }

        for (const auto &[from, to, duration] : layer.transitions)
        {
            auto *from_state = get_state(from, states_[i]);
            auto *to_state = get_state(to, states_[i]);

            from_state->transitions()[to_state->animation()->name()] = {to_state, duration};
        }

        ensure(current_state_[i] != nullptr, "no valid start start");
        current_state_[i]->enter();
    }
}

AnimationController::~AnimationController() = default;

void AnimationController::update()
{
    for (auto i = 0u; i < layers_.size(); ++i)
    {
        auto *next_state = current_state_[i]->update();
        if (next_state != nullptr)
        {
            current_state_[i]->exit();
            current_state_[i] = next_state;
            current_state_[i]->enter();
        }
    }
}

void AnimationController::play(std::size_t layer, std::string_view animation)
{
    current_state_[layer]->transition(animation);
}

}
