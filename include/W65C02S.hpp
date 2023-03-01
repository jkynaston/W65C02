#ifndef W65C02S_HPP
#define W65C02S_HPP

#include <bitset>
#include <cstdint>
#include <iomanip>
#include <stdexcept>

#define FLAG_N      0b10000000
#define FLAG_V      0b01000000
#define FLAG_B      0b00010000
#define FLAG_D      0b00001000
#define FLAG_I      0b00000100
#define FLAG_Z      0b00000010
#define FLAG_C      0b00000001
#define P_DEFAULT   0b00110100

#define W65C02S_RES 0b100
#define W65C02S_NMI 0b010
#define W65C02S_IRQ 0b001

#define GET_C()     P &FLAG_C
#define GET_Z()     P &FLAG_Z
#define GET_I()     P &FLAG_I
#define GET_D()     P &FLAG_D
#define GET_B()     P &FLAG_B
#define GET_V()     P &FLAG_V
#define GET_N()     P &FLAG_N

#define SET_C()     P |= FLAG_C
#define SET_Z()     P |= FLAG_Z
#define SET_I()     P |= FLAG_I
#define SET_D()     P |= FLAG_D
#define SET_B()     P |= FLAG_B
#define SET_V()     P |= FLAG_V
#define SET_N()     P |= FLAG_N

#define UNSET_C()   P &= ~FLAG_C
#define UNSET_Z()   P &= ~FLAG_Z
#define UNSET_I()   P &= ~FLAG_I
#define UNSET_D()   P &= ~FLAG_D
#define UNSET_B()   P &= ~FLAG_B
#define UNSET_V()   P &= ~FLAG_V
#define UNSET_N()   P &= ~FLAG_N

#define ENDF()                                                                                                         \
  addr_pins = PC;                                                                                                      \
  SYNC      = true;                                                                                                    \
  break;

class W65C02S
{
private:
  uint8_t  data_pins = 0x00;
  uint16_t addr_pins = 0x0000;

public:
  W65C02S() {}

  // registers
  uint8_t  A  = 0x00;      // accumulator
  uint8_t  X  = 0x00;      // index x register
  uint8_t  Y  = 0x00;      // index y register
  uint8_t  P  = P_DEFAULT; // status register
  uint16_t SP = 0x01FF;    // stack pointer
  uint16_t PC = 0x0000;    // program counter NV-BDIZC

  // status pins
  bool SYNC = true;
  bool RW   = true;
  bool RES  = true;

  // public functions
  void tick();

public:                    // internal "registers"
  uint8_t  IRV   = 0x00;   // interrupt value register (bit 2 = RESET, bit 1 = NMI, bit 0 = IRQ)
  uint16_t IR    = 0x0000; // instruction register
  uint16_t AR    = 0x0000; // internal address register
  uint32_t ticks = -1;     // total number of clock cycles elapsed

public:
  bool is_write() { return !RW; }

  void set_read() { RW = true; }

  void set_write() { RW = false; }

  bool is_reset() const { return !RES; }

  void set_reset() { RES = false; }

  uint16_t get_addr() const { return addr_pins; }

  void set_addr(const uint16_t addr_pins_) { addr_pins = addr_pins_; }

  uint8_t get_data() const { return data_pins; }

  void set_data(const uint8_t data_pins_) { data_pins = data_pins_; }

  std::string traceline()
  {
    auto strpf = [](std::string format, uint64_t val) {
      const char *format_arr = format.c_str();
      auto        size       = std::snprintf(nullptr, 0, format_arr, val);
      std::string output(size + 1, '\0');
      std::sprintf(&output[0], format_arr, val);
      return output;
    };
    std::string P_str = std::bitset<8>(P).to_string();
    P_str[2]          = '-';
    std::string str   = strpf("%6i", ticks) + " " + strpf("%04x", addr_pins) + " " + strpf("%02x", data_pins) + " " +
                      strpf("%2x", RW) + " " + strpf("%04x", PC) + " " + strpf("%02x", A) + " " + strpf("%3x", IR) +
                      " " + strpf("%02x", X) + " " + strpf("%02x", Y) + " " + strpf("%02x", SP) + " " + P_str;

    return str;
  }

  std::string tracefirst()
  {
    std::stringstream str;
    str << " cycle  ab  db rw  pc   a  ir  x  y sp    p    " << std::endl;
    str << "-----------------------------------------------" << std::endl;
    return str.str();
  }

private: // internal ops
  void _FETCH()
  {
    addr_pins = PC;
    SYNC      = true;
  }

  void _ADC();

  // instruction set
  /*
    TODO: New addressing modes
          [absolute indexed indirect] and [zero page indirect] exist for:
          ORA, AND, EOR, ADC, STA, LDA, CMP, SBC
   */
  void ADC();
  void AND();
  void ASL(); // TODO implement ASL
  void BCC(); // TODO implement BCC
  void BCS(); // TODO implement BCS
  void BEQ(); // TODO implement BEQ
  void BIT(); // TODO implement BIT
  void BMI(); // TODO implement BMI
  void BNE(); // TODO implement BNE
  void BPL(); // TODO implement BPL
  void BRK();
  void BVC(); // TODO implement BVC
  void BVS(); // TODO implement BVS
  void CLC();
  void CLD(); // TODO implement CLD
  void CLI(); // TODO implement CLI
  void CLV(); // TODO implement CLV
  void CMP(); // TODO implement CMP
  void CPX(); // TODO implement CPX
  void CPY(); // TODO implement CPY
  void DEC(); // TODO implement DEC
  void DEX(); // TODO implement DEX
  void DEY(); // TODO implement DEY
  void EOR(); // TODO implement EOR
  void INC(); // TODO implement INC
  void INX(); // TODO implement INX
  void INY(); // TODO implement INY
  void JMP(); // TODO implement JMP
  void JSR(); // TODO implement JSR
  void LDA();
  void LDX();
  void LDY();
  void LSR(); // TODO implement LSR
  void NOP(); // TODO implement NOP
  void ORA(); // TODO implement ORA
  void PHA(); // TODO implement PHA
  void PHP(); // TODO implement PHP
  void PLA(); // TODO implement PLA
  void PLP(); // TODO implement PLP
  void ROL(); // TODO implement ROL
  void ROR(); // TODO implement ROR
  void RTI(); // TODO implement RTI
  void RTS(); // TODO implement RTS
  void SBC(); // TODO implement SBC
  void SEC(); // TODO implement SEC
  void STA(); // TODO implement STA
  void STX(); // TODO implement STX
  void STY(); // TODO implement STY
  void TAX(); // TODO implement TAX
  void TAY(); // TODO implement TAY
  void TSX(); // TODO implement TSX
  void TXA(); // TODO implement TXA
  void TXS(); // TODO implement TXS
  void TYA(); // TODO implement TYA
};

#endif // W65C02S_HPP