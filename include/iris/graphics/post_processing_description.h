////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <optional>

namespace iris
{

struct AmbientOcclusion
{
};

struct Bloom
{
    float threshold = 1.0f;
    std::uint32_t iterations = 5u;
};

struct ToneMap
{
};

struct GammaCorrect
{
    float gamma = 2.2f;
};

struct AntiAliasing
{
};

struct PostProcessingDescription
{
    std::optional<AmbientOcclusion> ambient_occlusion = std::nullopt;
    std::optional<Bloom> bloom = std::nullopt;
    std::optional<ToneMap> tone_map = std::nullopt;
    std::optional<GammaCorrect> gamma_correct = std::nullopt;
    std::optional<AntiAliasing> anti_aliasing = std::nullopt;
};

}
