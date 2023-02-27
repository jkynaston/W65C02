#include "w65c02.hpp"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <iostream>

#define PROJECT_NAME "W65C02"

void W65C02::tick()
{
  if (SYNC) {
    IR   = D_BUS << 4;
    SYNC = false;
    PC++;
  }
  switch (IR++) {
    /* #region ADC */
  /* --------------------------------------------------- ADC zp ----------------------------------------------------- */
  // FIXME: unfinished
  case (0x65 << 4) | 0:
    A_BUS = PC++;
    END();
  /* --------------------------------------------------- ADC imm ---------------------------------------------------- */
  // DONE: tested
  case (0x69 << 4) | 0:
    A_BUS = PC++;
    END();
  case (0x69 << 4) | 1:
    _ADC(D_BUS);
    ENDF();
  /* --------------------------------------------------- ADC abs ---------------------------------------------------- */
  // DONE: tested
  case (0x6D << 4) | 0:
    A_BUS = PC++;
    END();
  case (0x6D << 4) | 1:
    AR    = D_BUS;
    A_BUS = PC++;
    END();
  case (0x6D << 4) | 2:
    AR |= D_BUS << 8;
    A_BUS = AR;
    END();
  case (0x6D << 4) | 3:
    _ADC(D_BUS);
    ENDF();
  /* -------------------------------------------------- ADC zp,X ---------------------------------------------------- */
  // DONE: tested
  case (0x75 << 4) | 0:
    A_BUS = PC++;
    END();
  case (0x75 << 4) | 1:
    AR = D_BUS;
    END();
  case (0x75 << 4) | 2:
    A_BUS = AR += X;
    END();
  case (0x75 << 4) | 3:
    _ADC(D_BUS);
    ENDF();
  /* -------------------------------------------------- ADC abs,X --------------------------------------------------- */
  // TODO: untested
  case (0x7D << 4) | 0:
    // request LO
    A_BUS = PC++;
    END();
  case (0x7D << 4) | 1:
    // request HI
    A_BUS = PC++;
    // read LO
    AR    = D_BUS;
    END();
  case (0x7D << 4) | 2:
    // read HI
    AR |= D_BUS << 8;
    // output incremented address (with page overflow)
    A_BUS = (AR & 0xFF00) | ((AR + X) & 0x00FF);
    // if no page crossed, skip to execution
    if (((AR + X) & 0xFF00) == (AR & 0xFF00)) IR++;
    END();
  case (0x7D << 4) | 3:
    // fix page overflow on boundary crossed
    A_BUS = (AR += X);
    END();
  case (0x7D << 4) | 4:
    // execute ADC
    _ADC(D_BUS);
    ENDF();
    /* #endregion */
  /* undefined behaviour */
  default:
    assert(false);
    break;
  }
}

void W65C02::println()
{
  auto strpf = [](std::string format, uint64_t val) {
    const char *format_arr = format.c_str();
    auto        size       = std::snprintf(nullptr, 0, format_arr, val);
    std::string output(size + 1, '\0');
    std::sprintf(&output[0], format_arr, val);
    return output;
  };
  std::string P_str     = std::bitset<8>(P).to_string();
  P_str[2]              = '-';
  std::string traceline = strpf("%6i", ticks) + " " + strpf("%04x", A_BUS) + " " + strpf("%02x", D_BUS) + " " +
                          strpf("%2x", RW) + " " + strpf("%04x", PC) + " " + strpf("%02x", A) + " " + strpf("%3x", IR) +
                          " " + strpf("%02x", X) + " " + strpf("%02x", Y) + " " + strpf("%02x", SP) + " " + P_str;

  std::cout << traceline << std::endl;
}

void W65C02::_ADC(uint8_t input)
{
  uint16_t result = (uint16_t)A + (uint16_t)input + (uint16_t)(GET_C());
  if (result > 255) P |= 0b00000001;
  if ((result & 0x00FF) == 0) {
    SET_Z();
  } else {
    UNSET_Z();
  }
  if (~((uint16_t)A ^ (uint16_t)input) & ((uint16_t)A ^ (uint16_t)result) & 0x0080) SET_V();
  if (result & 0x80) SET_N();

  A = result & 0xFF;
}

void W65C02::printfirst()
{
  std::cout << " cycle  ab  db rw  pc   a  ir  x  y sp    p    " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
}