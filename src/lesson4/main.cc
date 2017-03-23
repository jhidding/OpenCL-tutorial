#include <png++/png.hpp>

#include "../fractal/common.hh"

void render_openmp()
{
    png::image<png::rgb_pixel> image(1024, 768);
    auto image_map = mandelbrot_c(512);

    double res = 3.0 / image.get_width();

    #pragma omp parallel
    for (unsigned j = 0; j < image.get_height(); ++j)
    {
        for (unsigned i = 0; i < image.get_width(); ++i)
        {
            complex z(-2.15 + i * res, j * res - (image.get_height() / 2.0 * res));
            Colour c = colour_map(image_map(z));
            image[j][i] = png::rgb_pixel(c.r, c.g, c.b);
        }
    }

    image.write("mandel.png");
}

int main(int argc, char **argv)
{
    render_openmp();
}

