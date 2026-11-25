#include "replay.hpp"
#include <iostream>

int main() {
    for (bool drop : {false, true}) {
        const auto frames = replay(drop);
        int naive_wrong = 0, guarded_wrong = 0, paused = 0;
        for (std::size_t i = 1; i < frames.size(); ++i) {
            const auto& before = frames[i-1];
            const int dt = frames[i].ms - before.ms;
            if (before.naive != before.exchange) naive_wrong += dt;
            if (!before.guarded) paused += dt;
            else if (*before.guarded != before.exchange) guarded_wrong += dt;
        }
        if (naive_wrong != (drop ? 300 : 0) || guarded_wrong != (drop ? 40 : 0)
            || paused != (drop ? 80 : 0) || frames.back().applied != 4) return 1;
    }

    auto initial = opening_book();
    SequencedBook consumer(initial);
    consumer.receive({3, Side::bid, 50, 5});
    consumer.receive({3, Side::bid, 50, 5});
    consumer.snapshot(1, initial); // Still missing sequence 2.
    if (consumer.available() || consumer.bid() || consumer.published_book()) return 2;
    consumer.snapshot(2, initial); // Buffered level becomes the best bid.
    consumer.receive({3, Side::bid, 50, 0}); // Old duplicate must not delete it.
    consumer.snapshot(1, initial); // Late snapshot must not roll it back.
    if (!consumer.available() || consumer.sequence() != 3 || consumer.bid() != 50) return 3;
    const auto* recovered = consumer.published_book();
    if (!recovered || recovered->levels(Side::bid).at(48) != 20 ||
        recovered->levels(Side::ask).at(51) != 14) return 5;

    Book empty;
    empty.apply({1, Side::ask, 52, 2});
    empty.apply({2, Side::ask, 51, 4});
    empty.apply({3, Side::ask, 51, 0});
    if (empty.bid() || empty.ask() != 52) return 4;
    std::cout << "Verified clean and lossy exposure, suppression, and snapshot recovery.\n";
}
