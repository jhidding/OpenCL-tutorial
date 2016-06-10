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

#include <random>
#include <chrono>

using namespace cl_tutorial;

int main(int argc, char **argv)
{
    std::vector<cl::Device> devices;
    cl::Context cx;
    std::tie(devices, cx) = get_default_gpu_context();

    cl::Program program = get_program_from_file(
        cx, devices, "src/kernels/lesson3.cl");

    // reserve memory for buffers
    int err_code;
    unsigned N = 100000000, M = N;
    std::vector<float> a_h(N), b_h(N);
    size_t a_bytesize = sizeof(float) * N;
    cl::Buffer a_d(cx, CL_MEM_READ_WRITE, a_bytesize);
    
    std::cerr << "Running with N = " << N << ", generating random numbers ...\n";
    // get seed from current time
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    // random numbers using Mersenne prime twister and a normal distribution
    auto rng = std::bind(std::normal_distribution<float>(), std::mt19937(seed));
    // generate!
    std::generate(a_h.begin(), a_h.end(), rng);
    std::cerr << "(ok)\n\n";

    // Create a CommandQueue
    cl::CommandQueue queue(cx, devices[0], CL_QUEUE_PROFILING_ENABLE);

    // get the correct kernel
    cl::Kernel sum_step_k(program, "sum_step", &err_code);
    checkErr(err_code, "at kernel construction: sum_step");

    // copy the numbers to the device (non-blocking)
    cl::Event write_evt;
    queue.enqueueWriteBuffer(a_d, CL_FALSE, 0, a_bytesize, a_h.data(),
        NULL, &write_evt);

    // event_list takes ownership of all events;
    std::vector<cl::Kernel>
        kernel_list;

    // We keep a wait_list that contains one element for each
    // next iteration. 
    std::vector<cl::Event> 
        wait_list  = { cl::Event(write_evt) };

    while (M > 1)
    {
        unsigned N_floor = M / 2, N_ceil = N_floor;
        if (M & 1) ++N_ceil;

        // If the list has odd size, we do nothing with the
        // middle element. The new array size is N_ceil.

        set_args(sum_step_k, a_d, N_ceil);

        cl::Event sum_step_evt;
        std::cerr << "Queueing sum_step with size " << N_floor << std::endl;
        err_code = queue.enqueueNDRangeKernel(
            sum_step_k, 0, cl::NDRange(N_floor), cl::NullRange, 
            &wait_list, &sum_step_evt);
        checkErr(err_code, "enqueueing sum_step");

        wait_list[0] = cl::Event(sum_step_evt);

        M = N_ceil;
    }
    std::cerr << "Done building queue ...\n\n";

    float result;
    queue.enqueueReadBuffer(a_d, CL_TRUE, 0, sizeof(float), &result,
        &wait_list, NULL);
    queue.enqueueReadBuffer(a_d, CL_TRUE, 0, a_bytesize, b_h.data(),
        &wait_list, NULL);
    queue.finish();

    for (unsigned i = 0; i < 5; ++i)
        std::cout << a_h[i] << " " << b_h[i] << std::endl;
    std::cout << "..." << " " << "..." << std::endl;
    for (unsigned i = N-5; i < N; ++i)
        std::cout << a_h[i] << " " << b_h[i] << std::endl;

    std::cout << std::endl 
              << "Number: " << N << std::endl
              << "Sum:    " << result << std::endl
              << "Mean:   " << result / N << std::endl;
    
    return EXIT_SUCCESS;
}

