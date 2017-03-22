#include "info.hh"

using namespace cl_tutorial;

/*! \brief Prints some information on the available OpenCL runtime.
 */
void cl_tutorial::print_opencl_info(
    std::vector<cl::Platform> const &platform_list)
{
    std::cerr << "Number of platforms: " << platform_list.size() << "\n\n";

    for (unsigned i = 0; i < platform_list.size(); ++i)
    {
        auto const &platform = platform_list[i];
        std::cerr << "== Nº " << i+1
                  << " =================================================================\n"
                  << "  Name:      "
                  << platform.getInfo<CL_PLATFORM_NAME>() << "\n"
                  << "  Vendor:    "
                  << platform.getInfo<CL_PLATFORM_VENDOR>() << "\n";

        std::vector<cl::Device> device_list;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &device_list);

        std::cerr << "  # devices: " << device_list.size() << "\n\n";

        for (unsigned j = 0; j < device_list.size(); ++j)
        {
            auto const &device = device_list[j];
            std::cerr << "  -- Nº " << j+1
                      << " ---------------------------------------------------------------\n"
                      << "    Name:             "
                      << device.getInfo<CL_DEVICE_NAME>() << "\n"
                      << "    Device version:   "
                      << device.getInfo<CL_DEVICE_VERSION>() << "\n"
                      << "    Driver version:   "
                      << device.getInfo<CL_DRIVER_VERSION>() << "\n"
                      << "    OpenCL version:   "
                      << device.getInfo<CL_DEVICE_OPENCL_C_VERSION>() << "\n"
                      << "    # Compute units:  "
                      << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << "\n\n";
        }
    }
}

