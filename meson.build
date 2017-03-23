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

executable('cl-info', cl_info_files, cl_util_files, dependencies : opencl_dep)
executable('lesson1', cl_util_files, lesson1_files, dependencies : opencl_dep)
executable('lesson2', cl_util_files, lesson2_files, dependencies : opencl_dep)
executable('lesson3', cl_util_files, lesson3_files, dependencies : opencl_dep)

executable('lesson4', cl_util_files, lesson4_files, fractal_files,
    dependencies : [opencl_dep, libpng_dep],
    cpp_args: ['-fopenmp'],
    link_args: ['-lgomp'],
    include_directories: local_include)

