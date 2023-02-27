#include "../src/w65c02.hpp"

#include <gtest/gtest.h>

using RAMChip = std::array<uint8_t, 1 << 16>;

void cycle(W65C02 &chip, RAMChip &RAM)
{
  chip.tick();
  if (chip.RW)
    chip.D_BUS = RAM[chip.A_BUS];
  else
    RAM[chip.A_BUS] = chip.D_BUS;
  chip.println();
}

TEST(InstructionTests, ADC)
{
  W65C02 chip;

  RAMChip RAM = {0x69, 0x0a, 0x6d, 0xba, 0xab, 0x75, 0x01, 0x7D, 0x00, 0x00};
  RAM[0xabba] = 0x01;

  chip.printfirst();
  chip.D_BUS = RAM[0];
  for (int i = 0; i < 2; i++)
    cycle(chip, RAM);

  EXPECT_EQ(chip.A, 0x0a);
  EXPECT_EQ(chip.P, P_DEFAULT & (~FLAG_Z));

  for (int i = 0; i < 4; i++)
    cycle(chip, RAM);

  EXPECT_EQ(chip.A, 0x0b);
  EXPECT_EQ(chip.P, P_DEFAULT);

  for (int i = 0; i < 4; i++)
    cycle(chip, RAM);

  EXPECT_EQ(chip.A, 0x15);
  EXPECT_EQ(chip.P, P_DEFAULT);

  for (int i=0; i<4; i++)
    cycle(chip, RAM);

  EXPECT_EQ(chip.A, 0x7E);
  EXPECT_EQ(chip.P, P_DEFAULT);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}