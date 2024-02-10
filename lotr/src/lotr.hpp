#pragma once

#include <cinttypes>
#include <functional>
#include <optional>
#include <string>

namespace lotr {

struct MordorPopulation
{
    std::uint64_t orc_count;
    std::uint32_t troll_count;
    std::uint32_t nazgul_count;
    bool sauron_alive;
};

struct ServiceCallbacks
{
    std::function<MordorPopulation()> population;
    std::function<std::optional<std::uint64_t>(const std::string&, float)> kill_orcs;
};

} // namespace lotr
