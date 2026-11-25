#pragma once
#include "book.hpp"
#include <array>

inline Book opening_book() {
    Book book;
    book.apply({0, Side::bid, 48, 20});
    book.apply({0, Side::bid, 49, 10});
    book.apply({0, Side::ask, 51, 14});
    return book;
}

struct Tick { int ms; Delta update; };
inline constexpr std::array<Tick, 4> tape{{
    {100, {1, Side::bid, 49, 0}},
    {140, {2, Side::ask, 51, 12}},
    {190, {3, Side::bid, 47, 6}},
    {300, {4, Side::ask, 51, 9}}
}};
