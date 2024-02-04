#include "sync_client.hpp"

#include <fmt/format.h>
#include <iostream>

int main()
{
    fmt::print("Lets's play LOTR\n");

    SyncClient client("127.0.0.1", 55550);

    std::string cmd;

    while (true) {
        fmt::print("Enter command, kill (k), population (p), quit (x)\n");
        std::cin >> cmd;
        if (cmd == "p") {
            client.population();
        } else if (cmd == "k") {
            client.kill_orcs("sting", 0.7f);
        } else if (cmd == "x") {
            break;
        } else {
            fmt::print("Unknown command\n");
        }
    }

    fmt::print("Bye\n");

    return EXIT_SUCCESS;
}
