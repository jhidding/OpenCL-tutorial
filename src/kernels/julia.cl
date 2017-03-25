__kernel void julia(
        global float *out, float x0, float y0, float res,
        float cx, float cy, unsigned max_it)
{
    unsigned i = get_global_id(0),
             j = get_global_id(1);

    size_t address = i + j * get_global_size(0);

    float zx = x0 + res * i,
          zy = y0 + res * j;

    bool done = false;
    float result = 1.0;

    for (unsigned k = 0; k < max_it; ++k)
    {
        float u = zx*zx - zy*zy,
              v = 2*zx*zy,
              w = zx*zx + zy*zy;

        result = (w > 4.0 && !done ? (float)k / max_it : result);
        done = done || w > 4.0;

        // if (w > 4.0)
        // {
        //     out[address] = (float)k / max_it;
        //     return;
        // }

        zx = u + cx;
        zy = v + cy;
    }

    out[address] = result;
    // out[address] = 1.0;
}

