project('W65C02', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++20'])

gtest_dep = dependency('gtest', required: true)

# exe = executable('w65c02', 'src/w65c02.cpp',
#   install : true)

# exe2 = executable('6502', 'm6502.cpp', install: true)

adc_tests = executable('adc_tests', ['tests/adc_tests.cpp', 'src/w65c02.cpp'], dependencies: gtest_dep)
test('ADC tests', adc_tests, suite: 'instructions')