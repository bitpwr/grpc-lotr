#pragma once

#include <cinttypes>
#include <optional>
#include <string>

namespace lotr {

struct Options
{
    std::string address{ "0.0.0.0" };
    std::uint16_t port{ 37001 };
};

std::optional<Options> parse(int argc, char** argv);

} // namespace lotr
