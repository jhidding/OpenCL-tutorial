#pragma once

#include "fancy_string.hh"

namespace Misc
{
    namespace console
    {
        using fancy::ptr;
        using fancy::Style;
        using colour::Colour;

        /* Console style generators */
        inline ptr fg(unsigned r, unsigned g, unsigned b) {
            return std::make_unique<Style>(format("\033[38;2;", r, ";", g, ";", b, "m"));
        }

        inline ptr fg(Colour const &c) {
            return fg(c.r, c.g, c.b);
        }

        inline ptr bg(unsigned r, unsigned g, unsigned b) {
            return std::make_unique<Style>(format("\033[48;2;", r, ";", g, ";", b, "m"));
        }

        inline ptr bg(Colour const &c) {
            return bg(c.r, c.g, c.b);
        }

        inline ptr reset() {
            return std::make_unique<Style>("\033[m");
        }
    }
}
