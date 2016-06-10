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
using std::chrono::duration_cast;

class Timer
{
    using clock = std::chrono::high_resolution_clock;
    using unit_of_time  = std::chrono::milliseconds;

    clock::time_point t1;
    std::string msg;

    public:
        Timer() {}

        void start(std::string const &msg_) 
        {
            msg = msg_;
            std::cerr << "<<< start timer [" << msg << "]\n";
            t1 = clock::now();
        }

        void stop() const
        { 
            clock::time_point t2 = clock::now();
            double duration = std::chrono::duration<double, std::milli>(t2 - t1).count();
            std::cerr << ">>> stop timer [" << msg << "]: " << duration << " ms\n";
        }
};

int main(int argc, char **argv)
{
    Timer timer;
    std::vector<cl::Device> devices;
    cl::Context cx;
    std::tie(devices, cx) = get_default_gpu_context();
    std::cerr << "\n";

    timer.start("compiling"); 
    cl::Program program = get_program_from_file(
        cx, devices, "src/kernels/lesson3.cl");
    timer.stop(); std::cerr << "\n";

    // reserve memory for buffers
    int err_code;
    unsigned N = 1L << 26, M = N;
    std::vector<float> a_h(N), b_h(N);
    size_t a_bytesize = sizeof(float) * N;
    cl::Buffer a_d(cx, CL_MEM_READ_WRITE, a_bytesize);
    
    std::cerr << "Running with N = " << N << ", generating random numbers ...\n";
    timer.start("initialisation");
    // get seed from current time
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    // random numbers using Mersenne prime twister and a normal distribution
    auto rng = std::bind(std::normal_distribution<float>(), std::mt19937(seed));
    // generate!
    std::generate(a_h.begin(), a_h.end(), rng);
    timer.stop();
    std::cerr << "(ok)\n\n";

    timer.start("adding up numbers");
    // Create a CommandQueue
    cl::CommandQueue queue(cx, devices[0], CL_QUEUE_PROFILING_ENABLE);

    // get the correct kernel
    cl::Kernel sum_step_k(program, "sum_step", &err_code);
    checkErr(err_code, "at kernel construction: sum_step");

    // copy the numbers to the device (non-blocking)
    cl::Event write_evt;
    queue.enqueueWriteBuffer(a_d, CL_FALSE, 0, a_bytesize, a_h.data(),
        NULL, &write_evt);

    // We keep a wait_list that contains one element for each
    // next iteration. 
    std::vector<cl::Event> 
        wait_list  = { write_evt };
    std::vector<std::tuple<unsigned, cl::Event>>
        event_list;

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

        event_list.emplace_back(N_floor, sum_step_evt);
        wait_list[0] = sum_step_evt;

        M = N_ceil;
    }
    std::cerr << "Done building queue ...\n";

    float result;
    queue.enqueueReadBuffer(a_d, CL_TRUE, 0, sizeof(float), &result,
        &wait_list, NULL);
    timer.stop();

    queue.enqueueReadBuffer(a_d, CL_TRUE, 0, a_bytesize, b_h.data(),
        &wait_list, NULL);
    queue.finish();
    std::cerr << "\n";

    for (unsigned i = 0; i < 5; ++i)
        std::cerr << a_h[i] << " " << b_h[i] << std::endl;
    std::cerr << "..." << " " << "..." << std::endl;
    for (unsigned i = N-5; i < N; ++i)
        std::cerr << a_h[i] << " " << b_h[i] << std::endl;
    std::cerr << std::endl;

    std::cout << "# Number: " << N << std::endl
              << "# Sum:    " << result << std::endl
              << "# Mean:   " << result / N << std::endl;
   
    std::cout << "# timing stats\n"
              << "# |  n-elements |       time(ms) |\n";
    for (auto &evt : event_list)
    {
        unsigned long t1, t2;
        std::tie(t1, t2) = get_runtime(std::get<1>(evt));
        std::cout << std::setw(16) << std::get<0>(evt) << " " 
                  << std::fixed << std::setprecision(6)
                  << std::setw(16) << double(t2 - t1) / 1e6 << "\n";
    }

    return EXIT_SUCCESS;
}

