#include "types.hh"
#include <iostream>

Colour colour_map(double x) {
    double r = (0.472-0.567*x+4.05*pow(x, 2))
                /(1.+8.72*x-19.17*pow(x, 2)+14.1*pow(x, 3)),
           g = 0.108932-1.22635*x+27.284*pow(x, 2)-98.577*pow(x, 3)
                +163.3*pow(x, 4)-131.395*pow(x, 5)+40.634*pow(x, 6),
           b = 1./(1.97+3.54*x-68.5*pow(x, 2)+243*pow(x, 3)
                -297*pow(x, 4)+125*pow(x, 5));

    return Colour(int(r*255), int(g*255), int(b*255));
}


