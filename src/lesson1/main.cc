#include "../base.hh"
#include "../cl-util/get_gpu_context.hh"
#include "../cl-util/compile.hh"

#include <utility>
#include <tuple>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <numeric>
#include <iomanip>

using namespace cl_tutorial;

int main(int argc, char **argv)
{
    std::vector<cl::Device> devices;
    cl::Context cx;
    std::tie(devices, cx) = get_default_gpu_context();

    cl::Program program = get_program_from_file(
        cx, devices, "src/kernels/lesson1.cl");

    unsigned N = 1000000;
    std::vector<float> a_h(N), b_h(N);
    std::iota(a_h.begin(), a_h.end(), 0);
    size_t a_bytesize = sizeof(float) * a_h.size();

    cl::Buffer a_d(cx, CL_MEM_READ_WRITE, a_bytesize);

    cl::CommandQueue queue(cx, devices[0]);
    queue.enqueueWriteBuffer(a_d, CL_TRUE, 0, a_bytesize, a_h.data());

    int err_code;
    cl::Kernel kernel(program, "square", &err_code);
    checkErr(err_code, "at kernel construction.");
    kernel.setArg(0, a_d);
    checkErr(err_code, "at kernel.setArg().");
    queue.enqueueNDRangeKernel(
        kernel, cl::NullRange, cl::NDRange(N), cl::NullRange);
    queue.finish();


    queue.enqueueReadBuffer(a_d, CL_TRUE, 0, a_bytesize, b_h.data());

    for (unsigned i = 0; i < 5; ++i)
        std::cout << std::setw(10) << a_h[i] << " " << std::setw(16) << b_h[i] << "\n";
    std::cout << std::setw(10) << "..." << " " << std::setw(16) << "..." << "\n";
    for (unsigned i = N-6; i < N; ++i)
        std::cout << std::setw(10) << a_h[i] << " " << std::setw(16) << b_h[i] << "\n";

    return EXIT_SUCCESS;
}

