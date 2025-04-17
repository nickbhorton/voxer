#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

std::tuple<
    std::vector<std::array<uint8_t, 4>>,
    std::array<std::array<uint8_t, 4>, 256>>
parse_vox_file(std::string const& file_name);
