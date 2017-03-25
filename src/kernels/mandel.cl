__kernel void mandel(global float *out, float x0, float y0, float res, unsigned max_it)
{
    unsigned i = get_global_id(0),
             j = get_global_id(1);

    size_t address = i + j * get_global_size(0);

    float cx = x0 + res * i,
          cy = y0 + res * j,
          zx = 0, zy = 0;

    for (unsigned k = 0; k < max_it; ++k)
    {
        float u = zx*zx - zy*zy,
              v = 2*zx*zy,
              w = zx*zx + zy*zy;

        if (w > 4.0)
        {
            out[address] = (float)k / max_it;
            return;
        }

        zx = u + cx;
        zy = v + cy;
    }

    out[address] = 1.0;
}
