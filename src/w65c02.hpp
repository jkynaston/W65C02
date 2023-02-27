#ifndef W65C02_HPP
#define W65C02_HPP

#include <bitset>
#include <cstdint>

#define FLAG_N    0b10000000
#define FLAG_V    0b01000000
#define FLAG_B    0b00010000
#define FLAG_D    0b00001000
#define FLAG_I    0b00000100
#define FLAG_Z    0b00000010
#define FLAG_C    0b00000001
#define P_DEFAULT 0b00110100

#define GET_C()   P &FLAG_C
#define GET_Z()   P &FLAG_Z
#define GET_I()   P &FLAG_I
#define GET_D()   P &FLAG_D
#define GET_B()   P &FLAG_B
#define GET_V()   P &FLAG_V
#define GET_N()   P &FLAG_N

#define SET_C()   P |= FLAG_C
#define SET_Z()   P |= FLAG_Z
#define SET_I()   P |= FLAG_I
#define SET_D()   P |= FLAG_D
#define SET_B()   P |= FLAG_B
#define SET_V()   P |= FLAG_V
#define SET_N()   P |= FLAG_N

#define UNSET_C() P &= ~FLAG_C
#define UNSET_Z() P &= ~FLAG_Z
#define UNSET_I() P &= ~FLAG_I
#define UNSET_D() P &= ~FLAG_D
#define UNSET_B() P &= ~FLAG_B
#define UNSET_V() P &= ~FLAG_V
#define UNSET_N() P &= ~FLAG_N

#define END()                                                                                                          \
  ticks++;                                                                                                             \
  break

#define ENDF()                                                                                                         \
  A_BUS = PC;                                                                                                          \
  SYNC  = true;                                                                                                        \
  END();

class W65C02
{
public:
  W65C02();

  // registers
  uint8_t  A  = 0x00;      // accumulator
  uint8_t  X  = 0x00;      // index x register
  uint8_t  Y  = 0x00;      // index y register
  uint8_t  P  = P_DEFAULT; // status register
  uint8_t  SP = 0xFD;      // stack pointer
  uint16_t PC = 0x0000;    // program counter NV-BDIZC

  // status pins
  bool SYNC = true;
  bool RW   = true;

  // I/O pins
  uint8_t  D_BUS = 0x00;   // databus
  uint16_t A_BUS = 0x0000; // operand address register

  // clock functions
  void tick();

  void printfirst();
  void println();

public:                    // internal state variables
  uint16_t IR    = 0x0000; // instruction register
  uint16_t AR    = 0x0000; // internal address register
  uint32_t ticks = -1;     // total number of clock cycles elapsed

public:
  void _FETCH();
  // Specific operations
  void _ADC(uint8_t input); // add with carry

  // // Addressing modes
  // // Here, AD is the internal address register. ADL and ADH refer to the lo and hi bits of AD, resp.
  // // (a) refers to the data found at address a
  // void _RAD(); // Read address from the two bytes following PC
  // void ABS();  /* Absolute a
  //                   Second byte forms ADL and third forms ADH */
  // void INX();  /* Absolute Indexed Indirect (a, x)
  //                 | Byte 2 -> ADL, Byte 3 -> ADH,
  //                 | X + AD -> AD,
  //                 | (AD) -> PC */
  // void ABX();  /* Absolute Indexed with X a, x
  //                 | Byte 2 -> ADL, Byte 3 -> ADH
  //                 | X + AD -> PC */
  // void ABY();  /* Absolute Indexed with Y a, y
  //                 | Byte 2 -> ADL, Byte 3 -> ADH
  //                 | Y + AD -> PC */
  // void IND();  /* Absolute Indirect (a)
  //                 | Byte 2 -> ADL, Byte 3 -> ADH
  //                 | (AD) -> PC */
  // void IMP();  /* Implied address. Operand implied by instruction. */
  // void IMM();  /* Immediate Addressing
  //                 | PC -> PC + 1 */
  // void REL();
  // void STK();
  // void ZPG();
  // void ZIX();
  // void ZPX();
  // void ZPY();
  // void ZIN();
  // void ZIY();
};

inline W65C02::W65C02() {}

#endif // W65C02_HPP