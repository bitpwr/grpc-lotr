#include "application.hpp"
#include "options.hpp"

#include <fmt/format.h>

int main(int argc, char** argv)
{
    const auto options = lotr::parse(argc, argv);

    if (!options) {
        return EXIT_FAILURE;
    }

    lotr::Application app{};

    try {
        app.run();
    } catch (const std::exception& e) {
        fmt::print("unexpected failure: {}\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
