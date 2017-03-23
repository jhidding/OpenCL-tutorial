#pragma once

#define CL_HPP_MINIMUM_OPENCL_VERSION 100
#define CL_HPP_TARGET_OPENCL_VERSION  120
#include <CL/cl2.hpp>

#include <iostream>
#include <exception>
#include <chrono>

namespace cl_tutorial
{
    inline void checkErr(cl_int err, std::string const &msg)
    {
        if (err != CL_SUCCESS)
        {
            std::cerr << "ERROR (" << err << "):" << msg << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    class exception: public std::exception
    {
        std::string msg;

        public:
            exception(std::string const &msg):
                msg(msg) {}

            virtual char const *what() const throw()
            {
                return msg.c_str();
            }

            virtual ~exception() throw() {}
    };

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
}

