#include "tape.hpp"
#include <iostream>

int main() {
    auto exchange = opening_book();
    for (auto tick : tape) {
        exchange.apply(tick.update);
        std::cout << tick.ms << " ms exchange bid " << exchange.bid().value() << '\n';
    }
    return exchange.bid() == 48 ? 0 : 1;
}
