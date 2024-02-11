#include "options.hpp"

#include <boost/program_options.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>

template<>
struct fmt::formatter<boost::program_options::options_description> : ostream_formatter
{};

namespace lotr {

std::optional<Options> parse(int argc, char** argv)
{
    Options options{};
    boost::program_options::options_description description{ "Allowed options:" };

    description.add_options()("help,h", "show this help message");
    description.add_options()(
      "address,a",
      boost::program_options::value<std::string>(&options.address)->default_value(options.address),
      "service listening address");
    description.add_options()("sync_port,p",
                              boost::program_options::value<std::uint16_t>(&options.sync_port)
                                ->default_value(options.sync_port),
                              "service synchronous port");
    description.add_options()("async_port,q",
                              boost::program_options::value<std::uint16_t>(&options.async_port)
                                ->default_value(options.async_port),
                              "service asynchronous port");

    boost::program_options::variables_map vm;
    try {
        boost::program_options::store(
          boost::program_options::parse_command_line(argc, argv, description), vm);
    } catch (const std::exception& e) {
        fmt::print("Error: {}\n", e.what());
        return std::nullopt;
    }

    boost::program_options::notify(vm);

    if (vm.count("help") || vm.count("h")) {
        fmt::print("{}\n", description);
        exit(EXIT_SUCCESS);
    }

    return options;
}

} // namespace lotr
