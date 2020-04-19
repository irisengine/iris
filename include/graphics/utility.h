#pragma once

#include <cstdint>
#include <filesystem>
#include <tuple>
#include <vector>

namespace eng::graphics::utility
{

/**
 * Load an image file.
 *
 * @param path
 *   Path to file to load.
 *
 * @returns
 *   Tuple of <data, width, height, number of channels>.
 */
std::tuple<
    std::vector<std::uint8_t>,
    std::uint32_t,
    std::uint32_t,
    std::uint32_t>
    load_image(const std::filesystem::path &path);

}

