#include "async_client.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <fmt/format.h>
#include <iostream>

class Application
{
public:
    Application() {}

    void run()
    {
        fmt::print(
          "Enter command, kill (k), population (p),\nsubscribe (s), unsubscribe (u), quit (x)\n");
        get_cmd();
        context.run();
    }

    void get_cmd()
    {
        boost::asio::async_read_until(
          input, inbuf, '\n', [this](boost::system::error_code ec, size_t) {
              if (ec) {
                  fmt::print("read error: {}\n", ec.message());
                  return;
              }

              std::istream src(&inbuf);
              std::string cmd;
              std::getline(src, cmd);

              if (cmd == "p") {
                  client.population(
                    [](const grpc::Status& status, const lotr::proto::MordorPopulation& response) {
                        if (!status.ok()) {
                            fmt::print("Failed to get population, error:  {}, {}\n",
                                       static_cast<int>(status.error_code()),
                                       status.error_message());
                            return;
                        }
                        fmt::print("Mordor population\n");
                        fmt::print(" Orcs: {}\n", response.orc_count());
                        fmt::print(" Trolls: {}\n", response.troll_count());
                        fmt::print(" Nazguls: {}\n", response.nazgul_count());
                    });
              } else if (cmd == "k") {
                  client.kill_orcs(
                    "Glamdring", 0.7f, [](const grpc::Status& status, std::uint64_t orcs_killed) {
                        if (!status.ok()) {
                            fmt::print("Failed to kill orcs, error:  {}, {}\n",
                                       static_cast<int>(status.error_code()),
                                       status.error_message());
                            return;
                        }
                        fmt::print("Yay, killed {} orcs\n", orcs_killed);
                    });
              } else if (cmd == "x") {
                  return;
              } else {
                  fmt::print("Unknown command: {}\n", cmd);
              }

              get_cmd();
          });
    }

private:
    boost::asio::io_context context{};
    boost::asio::posix::stream_descriptor input{ context, STDIN_FILENO };
    boost::asio::streambuf inbuf{};
    AsyncClient client{ context, "127.0.0.1", 55555 };
};

int main()
{
    fmt::print("Lets's play LOTR\n");

    Application app;
    app.run();

    fmt::print("Bye\n");

    return EXIT_SUCCESS;
}
