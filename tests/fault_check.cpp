#include "consumer.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
constexpr int updates = 64;
constexpr int levels = 8;
using Depth = std::array<int, levels * 2>;
enum class Kind { deliver, loss, snapshot };
struct Event { int tick, seq; Kind kind; };

Side side_of(int slot) { return slot < levels ? Side::bid : Side::ask; }
int price_of(int slot) { return slot < levels ? 49 - slot : 51 + slot - levels; }

struct Scenario {
    // The exchange stores one integer per price. It never uses Book::apply.
    std::array<Depth, updates + 1> history{};
    std::array<Delta, updates + 1> deltas{};
    std::vector<Event> events;

    explicit Scenario(std::uint32_t seed) {
        std::mt19937 rng(seed);
        auto draw = [&](unsigned limit) { return static_cast<int>(rng() % limit); };
        for (int seq = 1; seq <= updates; ++seq) {
            const int slot = draw(levels * 2);
            const int size = draw(4) == 0 ? 0 : 1 + draw(40);
            history[seq] = history[seq - 1];
            history[seq][slot] = size;
            deltas[seq] = {seq, side_of(slot), price_of(slot), size};

            const int tick = seq * 2;
            if (draw(7) == 0) {
                events.push_back({tick, seq, Kind::loss});
            } else {
                const int arrival = tick + (draw(4) == 0 ? draw(10) : 0);
                events.push_back({arrival, seq, Kind::deliver});
                if (draw(5) == 0) events.push_back({arrival + draw(14), seq, Kind::deliver});
            }
            if (seq % 8 == 0) events.push_back({tick + draw(32), seq, Kind::snapshot});
        }
        // The final authoritative snapshot must restore publication after all faults.
        events.push_back({updates * 2 + 40, updates, Kind::snapshot});
        std::stable_sort(events.begin(), events.end(),
                         [](Event a, Event b) { return a.tick < b.tick; });
    }

    Book snapshot(int seq) const {
        Book book;
        for (int i = 0; i < levels * 2; ++i) {
            if (history[seq][i]) book.apply({seq, side_of(i), price_of(i), history[seq][i]});
        }
        return book;
    }

    void print(Event e) const {
        std::cerr << "tick=" << e.tick << " seq=" << e.seq;
        if (e.kind == Kind::snapshot) { std::cerr << " SNAPSHOT\n"; return; }
        const auto d = deltas[e.seq];
        std::cerr << (e.kind == Kind::loss ? " DROP " : " DELIVER ")
                  << (d.side == Side::bid ? "bid " : "ask ") << d.price << " size=" << d.size << '\n';
    }
};

// Track which sequence numbers arrived, independently of the consumer's buffer.
struct Coverage {
    std::array<bool, updates + 1> received{};
    int prefix = 0, highest = 0;
    bool paused = false;

    void observe(Event e) {
        if (e.kind == Kind::loss || e.seq < prefix) return;
        highest = std::max(highest, e.seq);
        if (e.kind == Kind::snapshot) {
            prefix = e.seq;
            while (prefix < highest && received[prefix + 1]) ++prefix;
            paused = prefix != highest;
        } else {
            received[e.seq] = true;
            if (!paused && e.seq == prefix + 1) ++prefix;
            paused = paused || highest > prefix;
        }
    }
};

void verify(const SequencedBook& consumer, const Coverage& coverage, const Depth& expected) {
    if (consumer.sequence() != coverage.prefix) throw std::runtime_error("sequence mismatch");
    if (consumer.available() == coverage.paused) throw std::runtime_error("availability mismatch");
    const auto* book = consumer.published_book();
    if (coverage.paused) {
        if (book || consumer.bid()) throw std::runtime_error("quote published during gap");
        return;
    }
    if (!book) throw std::runtime_error("missing published book");
    for (int offset : {0, levels}) {
        const auto& actual = book->levels(side_of(offset));
        std::size_t count = 0;
        for (int i = offset; i < offset + levels; ++i) {
            if (expected[i] == 0) continue;
            ++count;
            const auto level = actual.find(price_of(i));
            if (level == actual.end() || level->second != expected[i])
                throw std::runtime_error("wrong depth at price " + std::to_string(price_of(i)));
        }
        if (count != actual.size()) throw std::runtime_error("unexpected price level");
    }
    std::optional<int> bid;
    for (int i = 0; i < levels && !bid; ++i) if (expected[i]) bid = price_of(i);
    if (consumer.bid() != bid) throw std::runtime_error("wrong published bid");
}

bool run(std::uint32_t seed, bool trace) {
    Scenario scenario(seed);
    SequencedBook consumer(Book{});
    Coverage coverage;
    std::size_t step = 0;
    try {
        for (const auto e : scenario.events) {
            if (trace) scenario.print(e);
            coverage.observe(e);
            if (e.kind == Kind::snapshot) consumer.snapshot(e.seq, scenario.snapshot(e.seq));
            if (e.kind == Kind::deliver) consumer.receive(scenario.deltas[e.seq]);
            verify(consumer, coverage, scenario.history[coverage.prefix]);
            ++step;
        }
        if (!consumer.available() || consumer.sequence() != updates)
            throw std::runtime_error("final snapshot failed to restore publication");
    } catch (const std::exception& error) {
        std::cerr << "FAIL seed=" << seed << " step=" << step << ": " << error.what()
                  << "\nReproduce: feed_fault_check --seed " << seed << '\n';
        if (!trace) for (std::size_t i = 0; i <= step && i < scenario.events.size(); ++i)
            scenario.print(scenario.events[i]);
        return false;
    }
    return true;
}
} // namespace

int main(int argc, char** argv) {
    if (argc == 3 && std::string(argv[1]) == "--seed") {
        const std::string input = argv[2];
        std::uint32_t seed{};
        const auto parsed = std::from_chars(input.data(), input.data() + input.size(), seed);
        if (parsed.ec == std::errc{} && parsed.ptr == input.data() + input.size())
            return run(seed, true) ? 0 : 1;
    }
    if (argc != 1) { std::cerr << "Usage: feed_fault_check [--seed UINT32]\n"; return 2; }
    for (std::uint32_t seed = 0; seed < 5000; ++seed) if (!run(seed, false)) return 1;
    std::cout << "5000 seeds passed; 320000 exchange updates.\n";
}
