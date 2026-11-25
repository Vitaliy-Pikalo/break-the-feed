#pragma once
#include <map>
#include <optional>

enum class Side { bid, ask };

struct Delta {
    int seq;
    Side side;
    int price;
    int size; // Absolute displayed quantity in contracts; zero deletes the level.
};

class Book {
public:
    void apply(Delta d) {
        auto& levels = d.side == Side::bid ? bids : asks;
        if (d.size == 0) levels.erase(d.price);
        else levels[d.price] = d.size;
    }

    std::optional<int> bid() const {
        if (bids.empty()) return {};
        return bids.rbegin()->first;
    }

    std::optional<int> ask() const {
        if (asks.empty()) return {};
        return asks.begin()->first;
    }

private:
    std::map<int, int> bids, asks;
};
