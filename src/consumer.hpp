#pragma once
#include "book.hpp"
#include <utility>

class SequencedBook {
public:
    explicit SequencedBook(Book initial) : book(std::move(initial)) {}

    void receive(Delta update) {
        if (update.seq <= last) return;
        if (stale || update.seq != last + 1) {
            stale = true;
            pending.emplace(update.seq, update);
            return;
        }
        book.apply(update);
        last = update.seq;
    }

    void snapshot(int sequence, Book replacement) {
        if (sequence < last) return;
        book = std::move(replacement);
        last = sequence;
        pending.erase(pending.begin(), pending.upper_bound(last));
        while (!pending.empty() && pending.begin()->first == last + 1) {
            book.apply(pending.begin()->second);
            ++last;
            pending.erase(pending.begin());
        }
        stale = !pending.empty();
    }

    std::optional<int> bid() const { return stale ? std::nullopt : book.bid(); }
    bool available() const { return !stale; }
    const Book* published_book() const { return stale ? nullptr : &book; }
    int sequence() const { return last; }

private:
    Book book;
    int last = 0;
    bool stale = false;
    std::map<int, Delta> pending;
};
