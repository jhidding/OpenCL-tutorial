#pragma once

#include <complex>
#include <functional>

using complex = std::complex<double>;
using function = std::function<complex (complex)>;
using predicate = std::function<bool (complex)>;
using unit_map = std::function<double (complex)>;

struct Colour {
    int r, g, b;

    Colour(int r_, int g_, int b_):
        r(r_), g(g_), b(b_) {}
};

