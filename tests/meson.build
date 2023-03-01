comp_6502 = find_program('vasm6502_oldstyle')

#! adc_test
adc_bin = custom_target(
  output : 'test_adc.bin',
  input : 'test_adc.6502.s',
  command : [comp_6502, '-Fbin', '-dotdir', '@INPUT@', '-o', '@OUTPUT@'],
  build_by_default : true
)

test_adc = executable('test_adc', ['test_adc.cpp', '../src/W65C02S.cpp'], 
  dependencies: gtest_dep, 
  include_directories : incdir
)

test('ADC tests', test_adc, suite: 'instructions')

#! x62256 test
x62256_bin = custom_target(
  output : 'test_X62256.bin',
  input : 'test_X62256.6502.s',
  command : [comp_6502, '-Fbin', '-dotdir', '@INPUT@', '-o', '@OUTPUT@'],
  build_by_default : true
)

test_x62256 = executable('test_X62256', 'test_X62256.cpp',
  dependencies : gtest_dep,
  include_directories : incdir
)

test('62256 tests', test_x62256, suite: 'hardware')