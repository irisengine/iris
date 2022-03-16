////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "graphics/utils.h"

#include <cstdint>
#include <vector>

#include "core/data_buffer.h"

namespace iris
{

std::vector<MipLevelData> generate_mip_maps(const MipLevelData &start_level)
{
    std::vector<MipLevelData> mip_maps{start_level};

    // each subsequent mipmap is half the size of the precedng one
    static constexpr auto scale = 2u;

    // next mip level dimensions
    auto width = start_level.width / scale;
    auto height = start_level.height / scale;

    while ((width > 0u) && (height > 0u))
    {
        // we work on RGBA images
        static constexpr auto stride = 4u;

        DataBuffer mip_data(width * height * stride);

        auto *dst = mip_data.data();
        const auto *src = reinterpret_cast<const std::uint8_t *>(mip_maps.back().data.data());

        for (auto y = 0u; y < height; ++y)
        {
            const auto proj_y = y * scale;

            for (auto x = 0u; x < width; ++x)
            {
                const auto proj_x = x * scale;

                // calculate average for RGBA components one at a time
                for (auto i = 0u; i < stride; ++i)
                {
                    // get the 2x2 kernel
                    const auto pixel_1 = +(src[((((proj_y + 0u) * width * scale) + (proj_x + 0u)) * stride) + i]);
                    const auto pixel_2 = +(src[((((proj_y + 1u) * width * scale) + (proj_x + 0u)) * stride) + i]);
                    const auto pixel_3 = +(src[((((proj_y + 0u) * width * scale) + (proj_x + 1u)) * stride) + i]);
                    const auto pixel_4 = +(src[((((proj_y + 1u) * width * scale) + (proj_x + 1u)) * stride) + i]);

                    // average the values
                    const auto pixel = (pixel_1 + pixel_2 + pixel_3 + pixel_4) / 4;

                    *dst = static_cast<std::byte>(pixel);
                    ++dst;
                }
            }
        }

        mip_maps.push_back({.data = std::move(mip_data), .width = width, .height = height});

        width /= 2;
        height /= 2;
    }

    return mip_maps;
}

}
