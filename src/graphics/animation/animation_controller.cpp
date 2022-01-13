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
    auto state = std::find_if(
        std::begin(states),
        std::end(states),
        [name](const auto &element) { return element->animation()->name() == name; });

    iris::expect(state != std::end(states), "missing state");

    return state->get();
}

}

namespace iris
{

struct AnimationController::implementation
{
    Skeleton *skeleton;
    std::vector<std::vector<std::unique_ptr<AnimationState>>> states;
    std::vector<Animation> animations;
    std::vector<AnimationLayer> layers;
    std::vector<AnimationState *> current_state;
    std::unique_ptr<CachedBoneQuery> query;
};

AnimationController::AnimationController(
    const std::vector<Animation> &animations,
    const std::vector<AnimationLayer> &layers,
    Skeleton &skeleton)
    : impl_(std::make_unique<implementation>())
{
    ensure(!layers.empty(), "must have at least one layer");
    ensure(layers.front().bone_mask.empty(), "base layer cannot have a mask");

    std::vector<std::set<std::string>> bone_masks{};

    for (auto i = 1u; i < layers.size(); ++i)
    {
        bone_masks.emplace_back(std::cbegin(layers[i].bone_mask), std::cend(layers[i].bone_mask));
    }

    impl_->skeleton = std::addressof(skeleton);
    impl_->animations = animations;
    impl_->layers = layers;
    impl_->query = std::make_unique<CachedBoneQuery>(impl_->skeleton, bone_masks);

    impl_->states.resize(impl_->layers.size());
    impl_->current_state.resize(impl_->layers.size());

    for (auto i = 0u; i < impl_->layers.size(); ++i)
    {
        const auto &layer = impl_->layers[i];
        impl_->current_state[i] = nullptr;

        for (auto &animation : impl_->animations)
        {
            impl_->states[i].emplace_back(
                std::make_unique<AnimationState>(std::addressof(animation), impl_->skeleton, impl_->query.get(), i));

            if (animation.name() == layer.start_animation)
            {
                impl_->current_state[i] = impl_->states[i].back().get();
            }
        }

        for (const auto &[from, to, duration] : layer.transitions)
        {
            auto *from_state = get_state(from, impl_->states[i]);
            auto *to_state = get_state(to, impl_->states[i]);

            from_state->transitions()[to_state->animation()->name()] = {to_state, duration};
        }

        ensure(impl_->current_state[i] != nullptr, "no valid start start");
        impl_->current_state[i]->enter();
    }
}

AnimationController::~AnimationController() = default;

void AnimationController::update()
{
    for (auto i = 0u; i < impl_->layers.size(); ++i)
    {
        auto *next_state = impl_->current_state[i]->update();
        if (next_state != nullptr)
        {
            LOG_DEBUG("ac", "{} -> {}", impl_->current_state[i]->animation()->name(), next_state->animation()->name());

            impl_->current_state[i]->exit();
            impl_->current_state[i] = next_state;
            impl_->current_state[i]->enter();
        }
    }
}

void AnimationController::play(std::size_t layer, std::string_view animation)
{
    impl_->current_state[layer]->transition(animation);
}

}
