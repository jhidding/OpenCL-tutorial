#include <cmath>

#include "../base.hh"
#include "../save_png.hh"

#include "../cl-util/get_gpu_context.hh"
#include "../cl-util/compile.hh"
#include "../cl-util/timing.hh"
#include "../cl-util/set_args.hh"

using namespace cl_tutorial;

extern void mandel(
    float *out, float x0, float y0, float res, unsigned max_it,
    unsigned i, unsigned j, unsigned width);

extern void julia(
    float *out, float x0, float y0, float res,
    float cx, float cy, unsigned max_it,
    unsigned i, unsigned j, unsigned width);

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
        cx, devices, "src/kernels/mandel.cl");
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

    print_runtime_msg(get_runtime(mandel_event), "render time without overhead");
}

int main(int argc, char **argv)
{
    Timer time;

    size_t width = 3840, height = 2160;
    size_t size = width * height;

    std::vector<float> mandel_h(size);

    time.start("render with openmp");
    render_openmp(width, height, 512, mandel_h.data());
    time.stop();

    save_png("julia1.png", width, height, mandel_h, colour_map::rainbow);

    std::fill(mandel_h.begin(), mandel_h.end(), 0.0);
    time.start("render with opencl");
    render_opencl(width, height, 512, mandel_h.data());
    time.stop();

    save_png("julia2.png", width, height, mandel_h, colour_map::rainbow);
}

