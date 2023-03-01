#include "W65C02S.hpp"
#include "X62256.hpp"

#include <fstream>
#include <gtest/gtest.h>

void cycle(W65C02S &chip, X62256 &RAM, X62256 &ROM)
{
  chip.tick();

  if (chip.RW) {
    uint16_t rd_addr = chip.get_addr();
    if (rd_addr < 1 << 15) {
      RAM.set_addr(rd_addr);
      RAM.set_OE();
      RAM.tick();
      chip.set_data(RAM.get_data());
    } else {
      rd_addr -= 1 << 15;
      ROM.set_addr(rd_addr);
      ROM.set_OE();
      ROM.tick();
      chip.set_data(ROM.get_data());
    }
  } else {
    RAM.set_addr(chip.get_addr());
    RAM.set_data(chip.get_data());
    RAM.set_WE();
    RAM.tick();
  }
  std::cout << chip.traceline() << std::endl;
}

TEST(InstructionTests, ADC)
{
  W65C02S chip;
  X62256  ram;
  X62256  rom;

  chip.set_reset();

  ram.flash_from_file("tests/test_adc.bin");
  rom.flash_from_file("tests/test_adc.bin", 1 << 15);

  chip.tracefirst();
  for (int i = 0; i < 50; i++)
    cycle(chip, ram, rom);

  EXPECT_EQ(chip.A, 0x1e);
  EXPECT_EQ(chip.P, P_DEFAULT);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}