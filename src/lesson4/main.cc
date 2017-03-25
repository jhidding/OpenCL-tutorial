#include <png++/png.hpp>
#include <cmath>

#include "../base.hh"
#include "../cl-util/get_gpu_context.hh"
#include "../cl-util/compile.hh"
#include "../cl-util/timing.hh"
#include "../cl-util/set_args.hh"

using namespace cl_tutorial;

struct Colour {
    int r, g, b;

    Colour(int r_, int g_, int b_):
        r(r_), g(g_), b(b_) {}
};


Colour colour_map(float x) {
    float r = (0.472-0.567*x+4.05*pow(x, 2))
              /(1.+8.72*x-19.17*pow(x, 2)+14.1*pow(x, 3)),
          g = 0.108932-1.22635*x+27.284*pow(x, 2)-98.577*pow(x, 3)
              +163.3*pow(x, 4)-131.395*pow(x, 5)+40.634*pow(x, 6),
          b = 1./(1.97+3.54*x-68.5*pow(x, 2)+243*pow(x, 3)
              -297*pow(x, 4)+125*pow(x, 5));

    return Colour(int(r*255), int(g*255), int(b*255));
}


void mandel(float *out, float x0, float y0, float res, unsigned max_it,
    unsigned i, unsigned j, unsigned width)
{
    size_t address = i + j * width;

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

void julia(float *out, float x0, float y0, float res,
    float cx, float cy, unsigned max_it,
    unsigned i, unsigned j, unsigned width)
{
    size_t address = i + j * width;

    float zx = x0 + res * i,
          zy = y0 + res * j;

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

void render_openmp(unsigned width, unsigned height, unsigned max_it, float *data)
{
    #pragma omp parallel
    for (unsigned j = 0; j < height; ++j)
    {
        for (unsigned i = 0; i < width; ++i)
        {
            // mandel(data, -2.15, -1.0, 2.0/height, max_it, i, j, width);
            julia(data, -2.2, -1.1, 2.2/height, -0.03, 0.7, max_it, i, j, width);
        }
    }
}

void render_opencl(
    unsigned width, unsigned height, unsigned max_it, float *data)
{
    Timer time;
    time.start("loading opencl and compiling");
    std::vector<cl::Device> devices;
    cl::Context cx;
    std::tie(devices, cx) = get_default_gpu_context();
    cl::Program program = get_program_from_file(
        cx, devices, "src/kernels/julia.cl");
    cl::CommandQueue queue(cx, devices[0], CL_QUEUE_PROFILING_ENABLE);
    time.stop();

    size_t size = width * height;
    size_t mandel_bytesize = size * sizeof(float);
    cl::Buffer mandel_d(cx, CL_MEM_READ_WRITE, mandel_bytesize);

    int err_code;
    float res = 2.4f / height;
    float x0 = -res * width / 2.0,
          y0 = -1.2;

    cl::Kernel mandel_k(program, "julia", &err_code);
    checkErr(err_code, "at kernel construction: mandel");
    set_args(mandel_k, mandel_d, x0, y0, res, -0.03f, 0.7f, max_it);
    // set_args(mandel_k, mandel_d, -2.5f, -1.0f, 2.0f/height, max_it);
    checkErr(err_code, "at kernel.setArg(): mandel");

    cl::Event mandel_event;
    queue.enqueueNDRangeKernel(
        mandel_k, 0, cl::NDRange(width, height),
        cl::NullRange, nullptr, &mandel_event);

    std::vector<cl::Event> mandel_wait_list = {
        mandel_event };

    queue.enqueueReadBuffer(mandel_d, CL_TRUE, 0, mandel_bytesize,
        data, &mandel_wait_list, NULL);
    queue.finish();
}

int main(int argc, char **argv)
{
    Timer time;

    size_t dim[2] = { 3840, 2160 };
    // size_t dim[2] = { 1200, 600 };
    size_t size = dim[0] * dim[1];

    std::vector<float> mandel_h(size);

    time.start("render with openmp");
    render_openmp(dim[0], dim[1], 512, mandel_h.data());
    time.stop();

    {
        time.start("write to png");
        png::image<png::rgb_pixel> image(dim[0], dim[1]);

        for (unsigned j = 0; j < image.get_height(); ++j)
        {
            for (unsigned i = 0; i < image.get_width(); ++i)
            {
                Colour c = colour_map(mandel_h[i + j*dim[0]]);
                image[j][i] = png::rgb_pixel(c.r, c.g, c.b);
            }
        }

        image.write("julia1.png");
        time.stop();
    }

    std::fill(mandel_h.begin(), mandel_h.end(), 0.0);
    time.start("render with opencl");
    render_opencl(dim[0], dim[1], 512, mandel_h.data());
    time.stop();

    {
        time.start("write to png");
        png::image<png::rgb_pixel> image(dim[0], dim[1]);

        for (unsigned j = 0; j < image.get_height(); ++j)
        {
            for (unsigned i = 0; i < image.get_width(); ++i)
            {
                Colour c = colour_map(mandel_h[i + j*dim[0]]);
                image[j][i] = png::rgb_pixel(c.r, c.g, c.b);
            }
        }

        image.write("julia2.png");
        time.stop();
    }
}

