#pragma once

#define CL_HPP_MINIMUM_OPENCL_VERSION 100
#define CL_HPP_TARGET_OPENCL_VERSION  120
#include <CL/cl2.hpp>

#include <iostream>
#include <exception>

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
}

