# The premeson.sh script generated meson build files in the source directories
# one level deep. These directories are scanned for .cc files, and collected
# in a Meson `files` object, all these directories are visited in the build
# file that is present in `src`.
#
# If the directory name contains any hyphen, it is replaced with an underscore,
# and the entire name gets the `_files` suffix.
#
# This means all .cc files in for example the `src/cl-util` folder are included
# using `cl_util_files`.

project('OpenCL-tutorial', 'cpp')
subdir('src')

opencl_dep = dependency('OpenCL')
libpng_dep = dependency('libpng')

local_include = include_directories('./include')

cl_util_static = static_library('cl-util', cl_util_files,
    cpp_args: ['-std=c++14'],
    dependencies: opencl_dep,
    include_directories: local_include)

executable('cl-info', cl_info_files,
    cpp_args: ['-std=c++14'],
    dependencies : opencl_dep,
    link_with: [cl_util_static],
    include_directories: local_include)

executable('lesson1', lesson1_files,
    cpp_args: ['-std=c++14'],
    dependencies : opencl_dep,
    link_with: [cl_util_static],
    include_directories: local_include)

executable('lesson2', lesson2_files,
    cpp_args: ['-std=c++14'],
    dependencies : opencl_dep,
    link_with: [cl_util_static],
    include_directories: local_include)

executable('lesson3', lesson3_files,
    cpp_args: ['-std=c++14'],
    dependencies : opencl_dep,
    link_with: [cl_util_static],
    include_directories: local_include)

executable('lesson4', lesson4_files, fractal_files,
    dependencies : [opencl_dep, libpng_dep],
    cpp_args: ['-std=c++14', '-fopenmp'],
    link_args: ['-lgomp'],
    link_with: [cl_util_static],
    include_directories: local_include)

