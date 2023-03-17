////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/utils.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "core/data_buffer.h"

#include <stb_image_write.h>

namespace iris
{

std::vector<MipLevelData> generate_mip_maps(const MipLevelData &start_level)
{
    std::vector<MipLevelData> mip_maps{start_level};

    // each subsequent mipmap is half the size of the preceding one
    static constexpr auto scale = 2u;

    // next mip level dimensions
    auto width = start_level.width / scale;
    auto height = start_level.height / scale;

    while ((width > 0u) && (height > 0u))
    {
        const auto scale_x = static_cast<float>(mip_maps.back().width) / static_cast<float>(width);
        const auto scale_y = static_cast<float>(mip_maps.back().height) / static_cast<float>(height);

        // we work on RGBA images
        static constexpr auto stride = 4u;

        DataBuffer mip_data(width * height * stride);

        auto *dst = mip_data.data();
        const auto *src = reinterpret_cast<const std::uint8_t *>(mip_maps.back().data.data());

        // simple downsize algorithm by skipping pixels, will produce aliasing
        for (auto y = 0u; y < height; ++y)
        {
            const auto y_nearest = static_cast<int>(std::floor(y * scale_y));

            for (auto x = 0u; x < width; ++x)
            {
                const auto x_nearest = static_cast<int>(std::floor(x * scale_x));

                std::memcpy(dst, src + ((y_nearest * mip_maps.back().width) + x_nearest) * stride, stride);
                dst += stride;
            }
        }

        mip_maps.push_back({.data = std::move(mip_data), .width = width, .height = height});

        width /= scale;
        height /= scale;
    }

    return mip_maps;
}

}
