#pragma once

#include <cinttypes>
#include <optional>
#include <string>

namespace lotr {

struct Options
{
    std::string address{ "0.0.0.0" };
    std::uint16_t sync_port{ 55550 };
    std::uint16_t async_port{ 55555 };
};

std::optional<Options> parse(int argc, char** argv);

} // namespace lotr
