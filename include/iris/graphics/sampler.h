////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <functional>
#include <type_traits>

#include "core/colour.h"
#include "core/utils.h"

namespace iris
{

/**
 * Enumeration of possible sampling address modes, which describes the behaviour when a texture is sampled outside of
 * the range (0, 0) to (1, 1).
 *
 * As an example consider a 3-pixel wide 1D texture: ABC
 */
enum class SamplerAddressMode : std::uint8_t
{
    /** Repeat texture. Pattern ABC|ABC */
    REPEAT,

    /** Repeat but mirror the texture with each repeat. Pattern ABC|CBA */
    MIRROR,

    /** Use the colour at the edge of the texture. Pattern ABC|CCC */
    CLAMP_TO_EDGE,

    /** Use a custom colour. Pattern ABC|XXX */
    CLAMP_TO_BORDER
};

/**
 * Enumeration of possible filters, which describe how texture pixels (texels) should be mapped to a texture
 * coordinate.
 */
enum class SamplerFilter : std::uint8_t
{
    /** Use the texel that is closes to the texture coordinate. */
    NEAREST,

    /** Interpolate the nearest texels. */
    LINEAR
};

/**
 * Struct describing all sampler parameters.
 */
struct SamplerDescriptor
{
    /** Address mode along S coord. */
    SamplerAddressMode s_address_mode = SamplerAddressMode::REPEAT;

    /** Address mode along T coord. */
    SamplerAddressMode t_address_mode = SamplerAddressMode::REPEAT;

    /** Address mode along R coord (only applies to cube maps). */
    SamplerAddressMode r_address_mode = SamplerAddressMode::REPEAT;

    /** Border colour if address mode is CLAMP_TO_BORDER. */
    Colour border_colour = {0.0f, 0.0f, 0.0f};

    /** Filter to us when down-sampling texture. */
    SamplerFilter minification_filter = SamplerFilter::LINEAR;

    /** Filter to us when up-sampling texture. */
    SamplerFilter magnification_filter = SamplerFilter::LINEAR;

    /** Whether to generate and use mipmaps. */
    bool uses_mips = true;

    /** Filter mode for selecting between mipmap levels (only valid if uses_mips is true). */
    SamplerFilter mip_filter = SamplerFilter::LINEAR;

    bool operator==(const SamplerDescriptor &) const = default;
    bool operator!=(const SamplerDescriptor &) const = default;
};

/**
 * Abstract class that encapsulates sampling a texture.
 */
class Sampler
{
  public:
    /**
     * Construct a new Samper.
     *
     * @param descriptor
     *   Description of sampler parameters.
     *
     * @param index
     *   Index into the global array of all allocated samplers.
     */
    Sampler(const SamplerDescriptor &descriptor, std::uint32_t index);

    virtual ~Sampler() = default;

    Sampler(const Sampler &) = delete;
    Sampler &operator=(const Sampler &) = delete;

    /**
     * Get sampler descriptor.
     *
     * @returns
     *   Sampler descriptor.
     */
    SamplerDescriptor descriptor() const;

    /**
     * Get index into global array of all allocated samplers.
     *
     * This is most useful for bindless texturing as the sampler knows where in the mapped sampler table to find itself.
     *
     * @returns
     *   Index of sampler.
     */
    std::uint32_t index() const;

  private:
    /** Sampler description. */
    SamplerDescriptor descriptor_;

    /** Index into the global array of all allocated samplers. */
    std::uint32_t index_;
};

}

// specialise std::hash for SamplerDescriptor
namespace std
{

template <>
struct hash<iris::SamplerDescriptor>
{
    size_t operator()(const iris::SamplerDescriptor &descriptor) const
    {
        return iris::combine_hash(
            static_cast<std::underlying_type_t<iris::SamplerAddressMode>>(descriptor.s_address_mode),
            static_cast<std::underlying_type_t<iris::SamplerAddressMode>>(descriptor.t_address_mode),
            static_cast<std::underlying_type_t<iris::SamplerAddressMode>>(descriptor.r_address_mode),
            descriptor.border_colour,
            static_cast<std::underlying_type_t<iris::SamplerFilter>>(descriptor.minification_filter),
            static_cast<std::underlying_type_t<iris::SamplerFilter>>(descriptor.magnification_filter),
            descriptor.uses_mips,
            static_cast<std::underlying_type_t<iris::SamplerFilter>>(descriptor.mip_filter));
    }
};

}
