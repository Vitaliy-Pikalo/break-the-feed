#include "tape.hpp"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    const bool drop = argc == 1 || std::string(argv[1]) == "--drop";
    if (argc > 2 || (!drop && std::string(argv[1]) != "--clean")) {
        std::cerr << "Usage: feed [--drop|--clean]\n";
        return 2;
    }
    auto exchange = opening_book(), naive = exchange;
    for (auto tick : tape) {
        exchange.apply(tick.update);
        if (!(drop && tick.update.seq == 1)) naive.apply(tick.update);
        std::cout << tick.ms << " ms  exchange bid " << *exchange.bid()
                  << "  naive bid " << *naive.bid() << '\n';
    }
    return 0;
}
