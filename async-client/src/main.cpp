#include "async_client.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <fmt/format.h>
#include <iostream>
#include <random>

float random_power()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(0.6f, 0.9f);

    return dist(gen);
}

class Application
{
public:
    Application() {}

    void run()
    {
        fmt::print("Enter command, kill (k), population (p), connected (c),\nsubscribe (s), "
                   "unsubscribe (u), quit (x,q)\n");
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
                  client.kill_orcs("Glamdring",
                                   random_power(),
                                   [](const grpc::Status& status, std::uint64_t orcs_killed) {
                                       if (!status.ok()) {
                                           fmt::print("Failed to kill orcs, error:  {}, {}\n",
                                                      static_cast<int>(status.error_code()),
                                                      status.error_message());
                                           return;
                                       }
                                       fmt::print("Yay, killed {} orcs\n", orcs_killed);
                                   });
              } else if (cmd == "c") {
                  fmt::print("Connected: {}\n", client.connected());
              } else if (cmd == "s") {
                  client.subscribeToStatus(
                    [](const auto& status) {
                        fmt::print("Mordor strength: {:.1f}%, orc count: {}\n",
                                   status.mordor_strenght() * 100,
                                   status.orc_count());
                    },
                    [](const auto& status) {
                        fmt::print("Status reader done: {}, {}\n",
                                   static_cast<int>(status.error_code()),
                                   status.error_message());
                    });
              } else if (cmd == "u") {
                  client.unsubscribeToStatus();
              } else if (cmd == "x" || cmd == "q") {
                  client.shutdown();
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
