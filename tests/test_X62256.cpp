#include "X62256.hpp"

#include <gtest/gtest.h>

TEST(HardwareTests, X62256)
{
  X62256 RAM;

  RAM.flash_from_file("tests/test_X62256.bin", 0);

  RAM.set_WE(true);
  RAM.write(0x0005, 0xAB);
  RAM.tick();

  RAM.set_WE(false);
  RAM.set_OE(true);
  RAM.set_data(0x00);
  RAM.tick();
  EXPECT_EQ(RAM.get_data(), 0xAB);

  RAM.set_addr(0x0007);
  RAM.set_OE();
  RAM.tick();
  EXPECT_EQ(RAM.get_data(), 0xCD);

  RAM.set_addr(0x0006);
  RAM.set_OE();
  RAM.tick();
  EXPECT_EQ(RAM.get_data(), 0xEF);
  
  RAM.set_addr(0x0007);
  RAM.set_OE(false);
  RAM.tick();
  EXPECT_NE(RAM.get_data(), 0xCD);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}