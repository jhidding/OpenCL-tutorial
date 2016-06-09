#include "get_gpu_context.hh"
#include <iostream>

using namespace cl_tutorial;

std::tuple<std::vector<cl::Device>, cl::Context> 
    cl_tutorial::get_default_gpu_context()
{
    std::cerr << "OpenCL initialisation ... \n";

    std::vector<cl::Platform> platform_list;
    cl::Platform::get(&platform_list); 
    checkErr(platform_list.size() != 0 ? CL_SUCCESS : -1,
        " cl::Platform::get failed to retrieve a platform;\n"
        "  is OpenCL correctly installed?"
        "\n(fail)\n");
    
    cl::Platform default_platform = platform_list[0];
    std::cerr << "|  Using platform: "
              << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

    std::vector<cl::Device> device_list;
    default_platform.getDevices(CL_DEVICE_TYPE_GPU, &device_list);
    checkErr(device_list.size() != 0 ? CL_SUCCESS : -1, 
        " cl::Platform::getDevices failed to find a GPU device;\n"
        "  do you have the correct device loader installed?"
        "\n(fail)\n");

    cl::Device default_device = device_list[0];
    std::cerr << "|  Using device:   "
              << default_device.getInfo<CL_DEVICE_NAME>() << "\n";

    std::cerr << "(ok)\n";
    cl::Context context({default_device});

    return std::make_tuple(device_list, context);
}

