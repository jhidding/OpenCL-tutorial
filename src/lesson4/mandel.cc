#include <cstddef>

void mandel(float *out, float x0, float y0, float res, unsigned max_it,
    unsigned i, unsigned j, unsigned width)
{
    size_t address = i + j * width;

    float cx = x0 + res * i,
          cy = y0 + res * j,
          zx = 0, zy = 0, w = 0;

    unsigned k = 0;
    while (k < max_it && w < 4.0)
    {
        float u = zx*zx - zy*zy,
              v = 2*zx*zy;

        zx = u + cx;
        zy = v + cy;

        w = zx*zx + zy*zy;
        ++k;
    }

    out[address] = (k == max_it ? 1.0 : (float)k / max_it);
}

void julia(float *out, float x0, float y0, float res,
    float cx, float cy, unsigned max_it,
    unsigned i, unsigned j, unsigned width)
{
    size_t address = i + j * width;

    float zx = x0 + res * i,
          zy = y0 + res * j,
          w = zx*zx + zy*zy;

    unsigned k = 0;
    while (k < max_it && w < 4.0)
    {
        float u = zx*zx - zy*zy,
              v = 2*zx*zy;

        zx = u + cx;
        zy = v + cy;

        w = zx*zx + zy*zy;
        ++k;
    }

    out[address] = (k == max_it ? 1.0 : (float)k / max_it);
}
