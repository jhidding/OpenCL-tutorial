#include "../base.hh"
#include "../cl-util/get_gpu_context.hh"
#include "../cl-util/compile.hh"
#include "../cl-util/timing.hh"
#include "../cl-util/set_args.hh"

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
        cx, devices, "src/kernels/lesson2.cl");

    unsigned N = 12345678;
    std::vector<int> a_h(N), b_h(N), c_h(N);
    size_t a_bytesize = sizeof(int) * N;

    cl::Buffer a_d(cx, CL_MEM_READ_WRITE, a_bytesize);
    cl::Buffer b_d(cx, CL_MEM_READ_WRITE, a_bytesize);
    cl::Buffer c_d(cx, CL_MEM_READ_WRITE, a_bytesize);

    cl::CommandQueue queue(cx, devices[0], CL_QUEUE_PROFILING_ENABLE);

    int err_code;
    cl::Kernel initVectorA_k(program, "initVectorA", &err_code);
    checkErr(err_code, "at kernel construction: initVectorA");
    set_args(initVectorA_k, a_d);
    checkErr(err_code, "at kernel.setArg(): initVectorA");

    cl::Kernel initVectorB_k(program, "initVectorB", &err_code);
    checkErr(err_code, "at kernel construction: initVectorB");
    set_args(initVectorB_k, b_d, N);
    checkErr(err_code, "at kernel.setArg(): initVectorB");

    cl::Kernel sum_k(program, "sum", &err_code);
    checkErr(err_code, "at kernel construction: sum");

    set_args(sum_k, c_d, b_d, a_d);

    /* this was:
        sum_k.setArg(0, c_d);
        sum_k.setArg(1, b_d);
        sum_k.setArg(2, a_d); */

    checkErr(err_code, "at kernel.setArg(): sum");

    cl::Event initVectorA_evt, initVectorB_evt;
    queue.enqueueNDRangeKernel(
        initVectorA_k, 0, cl::NDRange(N), cl::NullRange, 
        NULL, &initVectorA_evt);
    queue.enqueueNDRangeKernel(
        initVectorB_k, 0, cl::NDRange(N), cl::NullRange, 
        NULL, &initVectorB_evt);

    std::vector<cl::Event> sum_wait_list = { 
        initVectorA_evt, initVectorB_evt };
    cl::Event sum_evt;
    queue.enqueueNDRangeKernel(
        sum_k, 0, cl::NDRange(N), cl::NullRange,
        &sum_wait_list, &sum_evt);

    std::vector<cl::Event> read_wait_list = { sum_evt };
    cl::Event read_a_evt;
    queue.enqueueReadBuffer(a_d, CL_TRUE, 0, a_bytesize, a_h.data(),
        &read_wait_list, NULL);
    queue.enqueueReadBuffer(b_d, CL_TRUE, 0, a_bytesize, b_h.data(),
        &read_wait_list, NULL);
    queue.enqueueReadBuffer(c_d, CL_TRUE, 0, a_bytesize, c_h.data(),
        &read_wait_list, NULL);
    queue.finish();

    /* write output */
    for (unsigned i = 0; i < 5; ++i)
        std::cout << std::setw(10) << a_h[i] << " " 
                  << std::setw(10) << b_h[i] << " "
                  << std::setw(16) << c_h[i] << "\n";

    std::cout << std::setw(10) << "..." << " " 
              << std::setw(10) << "..." << " "
              << std::setw(16) << "..." << "\n";

    for (unsigned i = N-5; i < N; ++i)
        std::cout << std::setw(10) << a_h[i] << " " 
                  << std::setw(10) << b_h[i] << " "
                  << std::setw(16) << c_h[i] << "\n";

    std::cerr << "\n\n";
    try
    {
        auto initVectorA_time = get_runtime(initVectorA_evt),
             initVectorB_time = get_runtime(initVectorB_evt),
             sum_time = get_runtime(sum_evt);

        print_runtime_msg(initVectorA_time, "== timing for 'initVectorA' ==");
        print_runtime_msg(initVectorB_time, "== timing for 'initVectorB' ==");
        print_runtime_msg(sum_time, "== timing for 'sum' ==");
    }
    catch (exception &expn)
    {
        std::cerr << "Exception: " << expn.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

