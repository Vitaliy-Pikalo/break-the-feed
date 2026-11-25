#include "replay.hpp"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    const std::string mode = argc > 1 ? argv[1] : "--drop";
    if (argc > 2 || (mode != "--drop" && mode != "--clean")) {
        std::cerr << "Usage: feed [--drop|--clean]\n";
        return 2;
    }
    const auto frames = replay(mode == "--drop");
    std::cout << "ms,event,exchange_bid,naive_bid,guarded_bid,applied_sequence\n";
    for (const auto& f : frames) {
        std::cout << f.ms << ',' << f.event << ',' << f.exchange << ',' << f.naive << ',';
        if (f.guarded) std::cout << *f.guarded;
        std::cout << ',' << f.applied << '\n';
    }
}
