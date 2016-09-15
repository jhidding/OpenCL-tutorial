OpenCL tutorial
===============

This repository contains some lessons on working with OpenCL.
The code is written in C++(14), and OpenCL C v1.2. It has only been tested
on my Debian machine with an integrated Intel Haswell using the open source 
Beignet drivers. 

### Contents

- cl-info: prints the information of all platforms and devices
  found on the host.
- lesson1: computes squares of an array of numbers.
- lesson2: generates two arrays on the device and computes the sum.
- lesson3: compute the sum of an array recursively.

### Build instructions

These lessons can be compiled using the Boil build tool. This build tool can be found
as an example application in the [Noodles repository](http://www.github.com/NLeSC/noodles);
and it is included here in the `scripts` directory. To install Noodles, do::
    
    pip3 install noodles --user

The build script reads `boil.ini`, which you should modify to fit you system's configuration.
The location of the OpenCL libraries is determined through `pkg-config`.

If you have a working CMakeLists.txt, I'd be happy to add it to the repository.

### Links

- [Khronos OpenCL homepage](https://www.khronos.org/opencl/)
- [OpenCL v2.1 Reference](https://www.khronos.org/registry/cl/sdk/2.1/docs/man/xhtml/)
- [OpenCL C++ bindings](http://github.khronos.org/OpenCL-CLHPP/index.html)

### Work in progress

I will keep working on these lessons as I familiarise myself with OpenCL.


Lessons
-------

### Lesson 0 (cl-info)

- OpenCL: finding devices

The OpenCL architecture has layers. It provides a complete abstraction for
machines with vector based (SIMD) parallelism. First of all, we determine the
platform that we're running on. Then we print information on every device
that this platform has. In the case of my laptop running Debian, this gives the
following output:

    $ ./cl-info
    Number of platforms: 1

    == Nº 1 =================================================================
      Name:      Intel Gen OCL Driver
      Vendor:    Intel
      # devices: 1

      -- Nº 1 ---------------------------------------------------------------
        Name:             Intel(R) HD Graphics Haswell Ultrabook GT2 Mobile
        Device version:   OpenCL 1.2 beignet 1.1.2
        Driver version:   1.1.2
        OpenCL version:   OpenCL C 1.2 beignet 1.1.2
        # Compute units:  20


### Lesson 1 (squares)

- Kernels: how to load, compile and run them.
- Buffers: allocating, reading and writing device memory.
- CommandQueue: queue kernels with arguments.

Here we have the first small OpenCL kernel, which we define in a separate source
file.

    __kernel void square(global float *buffer)
    {
        int i = get_global_id(0);
        buffer[i] = buffer[i] * buffer[i];
    }

This file is loaded into a `std::string` and then compiled to a `cl::Program`.
We define a buffer to run this kernel on. Since we do the I/O in blocking mode,
there are no concurrency issues yet. Each action on the device is send through
the *command queue*.


### Lesson 2 (adding up numbers)

- Manage dependencies with events.

Now we have several kernels which need to be run sequentially. First we have
two kernels that fill the device buffer with a range of numbers. Then we add
these numbers to create a third array. This last operation needs the first two
to have finished, before we can start. Each time we enter a command to the queue,
we can tie an *event* to it and a list of events on which we need to wait.

### Lesson 3 (summing a list of numbers)

- Generate a series of events programmatically.

To sum a list of numbers, we have a kernel that sums a number at the `global_id` index
and a number at a given offset. This kernel is then applied several times, each time
halving the size of the problem.

