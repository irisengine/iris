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

/**
 * User settings for SSAO.
 */
struct AmbientOcclusionDescription
{
    /** Number of samples to take when calculating occlusion. */
    std::uint32_t sample_count = 32u;

    /** Radius of each sample. */
    float radius = 0.5f;

    /** Bias to apply to random sample depth when comparing. */
    float bias = 0.025f;
};

/**
 * User settings for bloom.
 */
struct BloomDescription
{
    /** Threshold luminance to bloom.  */
    float threshold = 1.0f;

    /** Number of blur iterations. */
    std::uint32_t iterations = 5u;
};

/**
 * Enumeration of possible tone map curves.
 */
enum class ToneMapCurve
{
    REINHARD
};

/**
 * User settings for colour adjustment.
 */
struct ColourAdjustDescription
{
    /** Gamma amount. */
    float gamma = 2.2f;

    /** Tone map curve. */
    ToneMapCurve tone_map_curve = ToneMapCurve::REINHARD;
};

/**
 * User settings for FXAA.
 */
struct AntiAliasingDescription
{
};

/**
 * Description of post processing effects to apply a render pass.
 */
struct PostProcessingDescription
{
    std::optional<AmbientOcclusionDescription> ambient_occlusion = std::nullopt;
    std::optional<BloomDescription> bloom = std::nullopt;
    std::optional<ColourAdjustDescription> colour_adjust = std::nullopt;
    std::optional<AntiAliasingDescription> anti_aliasing = std::nullopt;
};

}
