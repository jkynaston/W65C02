#ifndef X62256_HPP
#define X62256_HPP

/*
    Cycle-stepped emulation of a generic 62256-family static RAM/ROM/EEPROM chip.
    ---
    Author: Joshua C. Kynaston
    Date: 2023-02-28
    Version: 0.1.0
    Modified: 2023-02-28
*/

#include <array>
#include <cstdint>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>

class X62256
{
private: // member variables
  std::array<uint8_t, 1 << 15> mem;
  uint16_t                     addr_pins = 0x0000;
  uint8_t                      data_pins = 0x00;
  bool                         CEb       = 0;
  bool                         OEb       = 1;
  bool                         WEb       = 1;

public: // interface
  void tick();

  void flash(std::array<uint8_t, 1 << 15> mem_) { mem = mem_; };

  void flash_from_file(std::string filename, int pos);

  void set_addr(const uint16_t &addr_pins_) { addr_pins = addr_pins_ & ~0x8000; }

  uint8_t get_data() const { return data_pins; }

  void set_data(const uint8_t &data_pins_) { data_pins = data_pins_; }

  void write(const uint16_t addr, const uint8_t data)
  {
    set_addr(addr);
    set_data(data);
  }

  void set_CE(bool c_e = true) { CEb = !c_e; }

  void set_OE(bool o_e = true) { OEb = !o_e; }

  void set_WE(bool w_e = true) { WEb = !w_e; }

  bool is_enabled() { return !CEb; };

  bool is_output() { return !OEb; };

  bool is_write() { return !WEb; };
};

inline void X62256::tick()
{
  if (is_enabled()) {
    if (is_write()) {
      mem[addr_pins] = get_data();
    } else if (is_output()) {
      set_data(mem[addr_pins]);
    }
  }

  set_OE(false);
  set_WE(false);
}

inline void X62256::flash_from_file(std::string filename, int pos = 0)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  char          mem_raw[1 << 15];
  if (in.is_open()) {
    in.seekg(pos, std::ios_base::beg);
    in.read(mem_raw, 1 << 15);
    for (int i = 0; i < 1 << 15; i++)
      mem[i] = mem_raw[i];
  } else {
    throw std::invalid_argument("File " + filename + " not found");
  }
}

#endif // X62256_HPP
