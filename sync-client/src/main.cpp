#include "sync_client.hpp"

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

int main()
{
    fmt::print("Lets's play LOTR\n");

    SyncClient client("127.0.0.1", 55550);
    std::string cmd;

    fmt::print("Enter command, kill (k), population (p), connected (c), quit (q,x)\n");

    while (true) {
        std::cin >> cmd;
        if (cmd == "p") {
            const auto pop = client.population();
            if (pop) {
                fmt::print("Mordor population:\n");
                fmt::print(" Orcs: {}\n", pop->orc_count());
                fmt::print(" Trolls: {}\n", pop->troll_count());
                fmt::print(" Nazguls: {}\n", pop->nazgul_count());
            }
        } else if (cmd == "k") {
            const auto orcs_killed = client.kill_orcs("sting", random_power());
            if (orcs_killed) {
                fmt::print("You killed {} orcs\n", *orcs_killed);
            }
        } else if (cmd == "c") {
            fmt::print("Connected: {}\n", client.connected());
        } else if (cmd == "x" || cmd == "q") {
            break;
        } else {
            fmt::print("Unknown command\n");
        }
    }

    fmt::print("Bye\n");

    return EXIT_SUCCESS;
}
