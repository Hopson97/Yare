#pragma once

#include <cstdint>

struct Colour {
    Colour() = default;

    Colour(uint8_t r, uint8_t g, uint8_t b)
        : r{r}
        , g{g}
        , b{b}
    {
    }

    Colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        : r{r}
        , g{g}
        , b{b}
        , a{a}
    {
    }

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 255;
};