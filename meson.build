project('W65C02S', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++20'])

add_global_arguments('-O1', language : 'cpp')

gtest_dep = dependency('gtest', required: true)

incdir = include_directories('include')
subdir('tests')