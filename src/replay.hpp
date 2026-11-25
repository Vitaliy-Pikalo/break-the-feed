#pragma once
#include "consumer.hpp"
#include "tape.hpp"
#include <vector>

struct Frame {
    int ms;
    const char* event;
    int exchange, naive;
    std::optional<int> guarded;
    int applied;
};

inline std::vector<Frame> replay(bool drop) {
    auto exchange = opening_book(), naive = exchange;
    SequencedBook guarded(exchange);
    Book saved;
    std::vector<Frame> frames;
    auto record = [&](int ms, const char* label) {
        frames.push_back({ms, label, *exchange.bid(), *naive.bid(), guarded.bid(),
                          guarded.sequence()});
    };
    record(0, "Opening snapshot");
    for (auto tick : tape) {
        const auto d = tick.update;
        exchange.apply(d);
        if (!(drop && d.seq == 1)) {
            naive.apply(d);
            guarded.receive(d);
        }
        switch (d.seq) {
        case 1: record(tick.ms, drop ? "49-cent deletion lost" : "49-cent deletion delivered"); break;
        case 2:
            saved = exchange;
            record(tick.ms, drop ? "Gap detected; snapshot requested" : "Ask size updated");
            break;
        case 3:
            record(tick.ms, drop ? "Sequence 3 buffered" : "47-cent bid added");
            if (drop) {
                guarded.snapshot(2, saved);
                record(220, "Snapshot 2 installed; sequence 3 replayed");
            }
            break;
        case 4: record(tick.ms, "Sequence 4 delivered"); break;
        }
    }
    record(400, "Observation ends");
    return frames;
}
