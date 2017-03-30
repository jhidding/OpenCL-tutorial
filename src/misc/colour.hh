#pragma once
#include <cstdint>

namespace Misc
{
    namespace colour
    {
        struct Colour
        {
            uint8_t r, g, b;
            Colour(uint8_t r, uint8_t g, uint8_t b):
                r(r), g(g), b(b) {}
        };

        typedef Colour Color;

        extern const Colour dark_gray;
        extern const Colour gentle_mauve;
        extern const Colour fluxom_lime;
    }
}

