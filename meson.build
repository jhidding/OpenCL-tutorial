project('OpenCL-tutorial', 'cpp')
subdir('src')

opencl_dep = dependency('OpenCL')
executable('cl-info', cl_info_files, cl_util_files, dependencies : opencl_dep)

