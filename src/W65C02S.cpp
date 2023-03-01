#include "W65C02S.hpp"

#include "utils.hpp"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <system_error>

void W65C02S::_ADC()
{
  uint8_t  data   = get_data();
  uint16_t result = (uint16_t)A + (uint16_t)data + (uint16_t)(GET_C());
  if (result > 255)
    SET_C();
  else
    UNSET_C();

  if ((result & 0x00FF) == 0)
    SET_Z();
  else
    UNSET_Z();

  if (((uint16_t)A ^ (uint16_t)data) & ((uint16_t)A ^ (uint16_t)result) & 0x80)
    SET_V();
  else
    UNSET_V();

  if (result & 0x80) {
    SET_N();
  } else
    UNSET_N();

  A = result & 0xFF;
}

void W65C02S::ADC()
{
  switch (IR) {
  // ADC (indirect,X)
  case (0x61 << 4) | 0:
    set_addr(PC++);
    break;
  case (0x61 << 4) | 1:
    AR = data_pins; // read operand
    set_addr(AR);   // output operand as address on bus
    break;
  case (0x61 << 4) | 2:
    AR += X;      // increment operand address by X
    AR &= 0xFF;   // implement wraparound
    set_addr(AR); // request LO byte of ptr
    break;
  case (0x61 << 4) | 3:
    addr_pins += 1;    // request HI byte of ptr
    addr_pins &= 0xFF; // wraparound
    AR = data_pins;    // write LO byte of ptr
    break;
  case (0x61 << 4) | 4:
    addr_pins = AR | (data_pins << 8); // read HI byte of ptr
    break;
  case (0x61 << 4) | 5:
    _ADC(); // adc on result
    _FETCH();
    break;
  // ADC zp
  case (0x65 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0x65 << 4) | 1:
    addr_pins = AR = data_pins;
    break;
  case (0x65 << 4) | 2:
    _ADC();
    _FETCH();
    break;
  // ADC imm
  case (0x69 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0x69 << 4) | 1:
    _ADC();
    _FETCH();
    break;
  // ADC abs
  case (0x6D << 4) | 0:
    addr_pins = PC++;
    break;
  case (0x6D << 4) | 1:
    AR        = data_pins;
    addr_pins = PC++;
    break;
  case (0x6D << 4) | 2:
    AR |= data_pins << 8;
    addr_pins = AR;
    break;
  case (0x6D << 4) | 3:
    _ADC();
    _FETCH();
    break;
  /* ---------------------------------------------- ADC (indirect),Y ------------------------------------------------ */
  case (0x71 << 4) | 0: // request operand
    set_addr(PC++);
    break;
  case (0x71 << 4) | 1:
    AR = get_data(); // read operand
    set_addr(AR);    // request (operand)
    break;
  case (0x71 << 4) | 2:
    set_addr((AR + 1) & 0xFF); // request (operand)_HI
    AR = get_data();           // read (operand)_LO
    break;
  case (0x71 << 4) | 3:
    AR |= get_data() << 8;                          // read ptr HI
    set_addr((AR & 0xFF00) | ((AR + Y) & 0x00FF));  // request ((operand)+Y) with pagewrap
    if ((AR & 0xFF00) == ((AR + Y) & 0xFF00)) IR++; // no pagewrap fix needed if no page crossed
    break;
  case (0x71 << 4) | 4:
    set_addr(AR + Y); // request ((operand)+Y) with pagewrap fixed
    break;
  case (0x71 << 4) | 5:
    _ADC(); // execute ADC
    _FETCH();
    break;
  /* -------------------------------------------------- ADC zp,X ---------------------------------------------------- */
  case (0x75 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0x75 << 4) | 1:
    AR = data_pins;
    break;
  case (0x75 << 4) | 2:
    addr_pins = AR += X;
    break;
  case (0x75 << 4) | 3:
    _ADC();
    _FETCH();
    break;
  /* -------------------------------------------------- ADC abs,Y ----------------------------------------------------*/
  case (0x79 << 4) | 0:
    // request LO
    addr_pins = PC++;
    break;
  case (0x79 << 4) | 1:
    // request HI
    addr_pins = PC++;
    // read LO
    AR        = data_pins;
    break;
  case (0x79 << 4) | 2:
    // read HI
    AR |= data_pins << 8;
    // output incremented address (with page overflow)
    addr_pins = (AR & 0xFF00) | ((AR + Y) & 0x00FF);
    // if no page crossed, skip to execution
    if (((AR + Y) & 0xFF00) == (AR & 0xFF00)) IR++;
    break;
  case (0x79 << 4) | 3:
    // fix page overflow on boundary crossed
    addr_pins = (AR += Y);
    break;
  case (0x79 << 4) | 4:
    // execute ADC
    _ADC();
    _FETCH();
    break;
  /* -------------------------------------------------- ADC abs,X --------------------------------------------------- */
  case (0x7D << 4) | 0:
    // request LO
    addr_pins = PC++;
    break;
  case (0x7D << 4) | 1:
    // request HI
    addr_pins = PC++;
    // read LO
    AR        = data_pins;
    break;
  case (0x7D << 4) | 2:
    // read HI
    AR |= data_pins << 8;
    // output incremented address (with page overflow)
    addr_pins = (AR & 0xFF00) | ((AR + X) & 0x00FF);
    // if no page crossed, skip to execution
    if (((AR + X) & 0xFF00) == (AR & 0xFF00)) IR++;
    break;
  case (0x7D << 4) | 3:
    // fix page overflow on boundary crossed
    addr_pins = (AR += X);
    break;
  case (0x7D << 4) | 4:
    // execute ADC
    _ADC();
    _FETCH();
    break;
  }
}

void W65C02S::AND()
{
  switch (IR) {
  /* ---------------------------------------------- AND (indirect,X) ------------------------------------------------ */
  case (0x21 << 4) | 0:
    set_addr(PC++);
    break;
  case (0x21 << 4) | 1:
    AR = data_pins; // read operand
    set_addr(AR);   // output operand as address on bus
    break;
  case (0x21 << 4) | 2:
    AR += X;      // increment operand address by X
    AR &= 0xFF;   // implement wraparound
    set_addr(AR); // request LO byte of ptr
    break;
  case (0x21 << 4) | 3:
    addr_pins += 1;    // request HI byte of ptr
    addr_pins &= 0xFF; // wraparound
    AR = data_pins;    // write LO byte of ptr
    break;
  case (0x21 << 4) | 4:
    addr_pins = AR | (data_pins << 8); // read HI byte of ptr
    break;
  case (0x21 << 4) | 5:
    A &= get_data();
    _FETCH();
    break;
  /* --------------------------------------------------- AND zp ----------------------------------------------------- */
  case (0x25 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0x25 << 4) | 1:
    addr_pins = AR = get_data();
    break;
  case (0x25 << 4) | 2:
    A &= get_data();
    _FETCH();
    break;
  /* --------------------------------------------------- AND imm ---------------------------------------------------- */
  case (0x29 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0x29 << 4) | 1:
    A &= get_data();
    _FETCH();
    break;
  /* --------------------------------------------------- AND abs ---------------------------------------------------- */
  case (0x2D << 4) | 0:
    addr_pins = PC++;
    break;
  case (0x2D << 4) | 1:
    AR        = data_pins;
    addr_pins = PC++;
    break;
  case (0x2D << 4) | 2:
    AR |= data_pins << 8;
    addr_pins = AR;
    break;
  case (0x2D << 4) | 3:
    A &= get_data();
    _FETCH();
    break;
  /* ---------------------------------------------- AND (indirect),Y ------------------------------------------------ */
  case (0x31 << 4) | 0: // request operand
    set_addr(PC++);
    break;
  case (0x31 << 4) | 1:
    AR = get_data(); // read operand
    set_addr(AR);    // request (operand)
    break;
  case (0x31 << 4) | 2:
    set_addr((AR + 1) & 0xFF); // request (operand)_HI
    AR = get_data();           // read (operand)_LO
    break;
  case (0x31 << 4) | 3:
    AR |= get_data() << 8;                          // read ptr HI
    set_addr((AR & 0xFF00) | ((AR + Y) & 0x00FF));  // request ((operand)+Y) with pagewrap
    if ((AR & 0xFF00) == ((AR + Y) & 0xFF00)) IR++; // no pagewrap fix needed if no page crossed
    break;
  case (0x31 << 4) | 4:
    set_addr(AR + Y); // request ((operand)+Y) with pagewrap fixed
    break;
  case (0x31 << 4) | 5:
    A &= get_data();
    _FETCH();
    break;
  /* -------------------------------------------------- AND zp,X ---------------------------------------------------- */
  case (0x35 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0x35 << 4) | 1:
    AR = data_pins;
    break;
  case (0x35 << 4) | 2:
    addr_pins = AR += X;
    break;
  case (0x35 << 4) | 3:
    A &= get_data();
    _FETCH();
    break;
  /* -------------------------------------------------- AND abs,Y ----------------------------------------------------*/
  case (0x39 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0x39 << 4) | 1:
    addr_pins = PC++;
    AR        = data_pins;
    break;
  case (0x39 << 4) | 2:
    AR |= data_pins << 8;
    addr_pins = (AR & 0xFF00) | ((AR + Y) & 0x00FF);
    if (((AR + Y) & 0xFF00) == (AR & 0xFF00)) IR++;
    break;
  case (0x39 << 4) | 3:
    addr_pins = (AR += Y);
    break;
  case (0x39 << 4) | 4:
    A &= get_data();
    _FETCH();
    break;
  /* -------------------------------------------------- AND abs,X --------------------------------------------------- */
  case (0x3D << 4) | 0:
    addr_pins = PC++;
    break;
  case (0x3D << 4) | 1:
    addr_pins = PC++;
    AR        = data_pins;
    break;
  case (0x3D << 4) | 2:
    AR |= data_pins << 8;
    addr_pins = (AR & 0xFF00) | ((AR + X) & 0x00FF);
    if (((AR + X) & 0xFF00) == (AR & 0xFF00)) IR++;
    break;
  case (0x3D << 4) | 3:
    addr_pins = (AR += X);
    break;
  case (0x3D << 4) | 4:
    A &= get_data();
    _FETCH();
    break;
  }

  if ((A & 0x80) != 0)
    SET_N();
  else
    UNSET_N();

  if (A == 0x00)
    SET_Z();
  else
    UNSET_Z();
}

void W65C02S::ASL() {}

void W65C02S::BCC() {}

void W65C02S::BCS() {}

void W65C02S::BEQ() {}

void W65C02S::BIT() {}

void W65C02S::BMI() {}

void W65C02S::BNE() {}

void W65C02S::BPL() {}

void W65C02S::BRK()
{
  switch (IR) {
  case (0x00 << 4) | 0:
    set_addr(PC);
    break;
  case (0x00 << 4) | 1:
    if ((IRV & W65C02S_RES) == 0) PC++;
    set_addr(SP--);
    set_data(PC >> 8);
    if ((IRV & W65C02S_RES) == 0) set_write();
    break;
  case (0x00 << 4) | 2:
    set_addr(SP--);
    set_data(PC);
    if ((IRV & W65C02S_RES) == 0) set_write();
    break;
  case (0x00 << 4) | 3:
    set_addr(SP--);
    set_data(P);
    if (IRV & W65C02S_RES) {
      AR = 0xFFFC;
    } else {
      set_write();
      if (IRV & W65C02S_NMI)
        AR = 0xFFFA;
      else
        AR = 0xFFFE;
    }
    break;
  case (0x00 << 4) | 4:
    set_addr(AR++);
    SET_I();
    SET_B();
    IRV = 0;
    break;
  case (0x00 << 4) | 5:
    set_addr(AR);
    AR = get_data();
    break;
  case (0x00 << 4) | 6:
    PC = (get_data() << 8) | AR;
    _FETCH();
    break;
  }
}

void W65C02S::BVC() {}

void W65C02S::BVS() {}

void W65C02S::CLC()
{
  switch (IR) {
  case (0x18 << 4) | 0:
    addr_pins = PC;
    break;
  case (0x18 << 4) | 1:
    UNSET_C();
    _FETCH();
    break;
  }
}

void W65C02S::LDA()
{
  switch (IR) {
  case (0xA1 << 4) | 0:
    set_addr(PC++);
    break;
  case (0xA1 << 4) | 1:
    AR = data_pins; // read operand
    set_addr(AR);   // output operand as address on bus
    break;
  case (0xA1 << 4) | 2:
    AR += X;      // increment operand address by X
    AR &= 0xFF;   // implement wraparound
    set_addr(AR); // request LO byte of ptr
    break;
  case (0xA1 << 4) | 3:
    addr_pins += 1;    // request HI byte of ptr
    addr_pins &= 0xFF; // wraparound
    AR = data_pins;    // write LO byte of ptr
    break;
  case (0xA1 << 4) | 4:
    addr_pins = AR | (data_pins << 8); // read HI byte of ptr
    break;
  case (0xA1 << 4) | 5:
    A = get_data();
    _FETCH();
    break;
  /* --------------------------------------------------- LDA zp ----------------------------------------------------- */
  case (0xA5 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xA5 << 4) | 1:
    addr_pins = AR = get_data();
    break;
  case (0xA5 << 4) | 2:
    A = get_data();
    _FETCH();
    break;
  /* --------------------------------------------------- LDA imm ---------------------------------------------------- */
  case (0xA9 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xA9 << 4) | 1:
    A = get_data();
    _FETCH();
    break;
  /* --------------------------------------------------- ADC abs ---------------------------------------------------- */
  case (0xAD << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xAD << 4) | 1:
    AR        = data_pins;
    addr_pins = PC++;
    break;
  case (0xAD << 4) | 2:
    AR |= data_pins << 8;
    addr_pins = AR;
    break;
  case (0xAD << 4) | 3:
    A = get_data();
    _FETCH();
    break;
  /* ---------------------------------------------- ADC (indirect),Y ------------------------------------------------ */
  case (0xB1 << 4) | 0: // request operand
    set_addr(PC++);
    break;
  case (0xB1 << 4) | 1:
    AR = get_data(); // read operand
    set_addr(AR);    // request (operand)
    break;
  case (0xB1 << 4) | 2:
    set_addr((AR + 1) & 0xFF); // request (operand)_HI
    AR = get_data();           // read (operand)_LO
    break;
  case (0xB1 << 4) | 3:
    AR |= get_data() << 8;                          // read ptr HI
    set_addr((AR & 0xFF00) | ((AR + Y) & 0x00FF));  // request ((operand)+Y) with pagewrap
    if ((AR & 0xFF00) == ((AR + Y) & 0xFF00)) IR++; // no pagewrap fix needed if no page crossed
    break;
  case (0xB1 << 4) | 4:
    set_addr(AR + Y); // request ((operand)+Y) with pagewrap fixed
    break;
  case (0xB1 << 4) | 5:
    A = get_data();
    _FETCH();
    break;
  /* -------------------------------------------------- LDA zp,X ---------------------------------------------------- */
  case (0xB5 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xB5 << 4) | 1:
    AR = data_pins;
    break;
  case (0xB5 << 4) | 2:
    addr_pins = AR += X;
    break;
  case (0xB5 << 4) | 3:
    A = get_data();
    _FETCH();
    break;
  /* -------------------------------------------------- LDA abs,Y ----------------------------------------------------*/
  case (0xB9 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xB9 << 4) | 1:
    addr_pins = PC++;
    AR        = data_pins;
    break;
  case (0xB9 << 4) | 2:
    AR |= data_pins << 8;
    addr_pins = (AR & 0xFF00) | ((AR + Y) & 0x00FF);
    if (((AR + Y) & 0xFF00) == (AR & 0xFF00)) IR++;
    break;
  case (0xB9 << 4) | 3:
    addr_pins = (AR += Y);
    break;
  case (0xB9 << 4) | 4:
    A = get_data();
    _FETCH();
    break;
  /* -------------------------------------------------- LDA abs,X --------------------------------------------------- */
  case (0xBD << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xBD << 4) | 1:
    addr_pins = PC++;
    AR        = data_pins;
    break;
  case (0xBD << 4) | 2:
    AR |= data_pins << 8;
    addr_pins = (AR & 0xFF00) | ((AR + X) & 0x00FF);
    if (((AR + X) & 0xFF00) == (AR & 0xFF00)) IR++;
    break;
  case (0xBD << 4) | 3:
    addr_pins = (AR += X);
    break;
  case (0xBD << 4) | 4:
    A = get_data();
    _FETCH();
    break;
  }
}

void W65C02S::LDX()
{
  switch (IR) {
  /* --------------------------------------------------- LDX zp ----------------------------------------------------- */
  case (0xA6 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xA6 << 4) | 1:
    addr_pins = AR = get_data();
    break;
  case (0xA6 << 4) | 2:
    X = get_data();
    _FETCH();
    break;
  /* --------------------------------------------------- LDX imm ---------------------------------------------------- */
  case (0xA2 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xA2 << 4) | 1:
    X = get_data();
    _FETCH();
    break;
  /* --------------------------------------------------- LDX abs ---------------------------------------------------- */
  case (0xAE << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xAE << 4) | 1:
    AR        = data_pins;
    addr_pins = PC++;
    break;
  case (0xAE << 4) | 2:
    AR |= data_pins << 8;
    addr_pins = AR;
    break;
  case (0xAE << 4) | 3:
    X = get_data();
    _FETCH();
    break;
  /* -------------------------------------------------- LDX zp,X ---------------------------------------------------- */
  case (0xB6 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xB6 << 4) | 1:
    AR = data_pins;
    break;
  case (0xB6 << 4) | 2:
    addr_pins = AR += X;
    break;
  case (0xB6 << 4) | 3:
    X = get_data();
    _FETCH();
    break;
  /* -------------------------------------------------- LDX abs,Y ----------------------------------------------------*/
  case (0xBE << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xBE << 4) | 1:
    addr_pins = PC++;
    AR        = data_pins;
    break;
  case (0xBE << 4) | 2:
    AR |= data_pins << 8;
    addr_pins = (AR & 0xFF00) | ((AR + Y) & 0x00FF);
    if (((AR + Y) & 0xFF00) == (AR & 0xFF00)) IR++;
    break;
  case (0xBE << 4) | 3:
    addr_pins = (AR += Y);
    break;
  case (0xBE << 4) | 4:
    X = get_data();
    _FETCH();
    break;
  }

  if ((A & 0x80) != 0)
    SET_N();
  else
    UNSET_N();

  if (A == 0)
    SET_Z();
  else
    UNSET_Z();
}

void W65C02S::LDY()
{
  switch (IR) {
  /* --------------------------------------------------- LDY zp ----------------------------------------------------- */
  case (0xA4 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xA4 << 4) | 1:
    addr_pins = AR = get_data();
    break;
  case (0xA4 << 4) | 2:
    Y = get_data();
    _FETCH();
    break;
  /* --------------------------------------------------- LDY imm ---------------------------------------------------- */
  case (0xA0 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xA0 << 4) | 1:
    Y = get_data();
    _FETCH();
    break;
  /* --------------------------------------------------- LDY abs ---------------------------------------------------- */
  case (0xAC << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xAC << 4) | 1:
    AR        = data_pins;
    addr_pins = PC++;
    break;
  case (0xAC << 4) | 2:
    AR |= data_pins << 8;
    addr_pins = AR;
    break;
  case (0xAC << 4) | 3:
    Y = get_data();
    _FETCH();
    break;
  /* -------------------------------------------------- LDY zp,X ---------------------------------------------------- */
  case (0xB4 << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xB4 << 4) | 1:
    AR = data_pins;
    break;
  case (0xB4 << 4) | 2:
    addr_pins = AR += X;
    break;
  case (0xB4 << 4) | 3:
    Y = get_data();
    _FETCH();
    break;
  /* -------------------------------------------------- LDY abs,X ----------------------------------------------------*/
  case (0xBC << 4) | 0:
    addr_pins = PC++;
    break;
  case (0xBC << 4) | 1:
    addr_pins = PC++;
    AR        = data_pins;
    break;
  case (0xBC << 4) | 2:
    AR |= data_pins << 8;
    addr_pins = (AR & 0xFF00) | ((AR + X) & 0x00FF);
    if (((AR + X) & 0xFF00) == (AR & 0xFF00)) IR++;
    break;
  case (0xBC << 4) | 3:
    addr_pins = (AR += X);
    break;
  case (0xBC << 4) | 4:
    Y = get_data();
    _FETCH();
    break;
  }

  if ((A & 0x80) != 0)
    SET_N();
  else
    UNSET_N();

  if (A == 0)
    SET_Z();
  else
    UNSET_Z();
}

void W65C02S::tick()
{
  if (SYNC) {
    IR   = data_pins << 4;
    SYNC = false;
    if (is_reset()) IRV |= W65C02S_RES;
    if (IRV) {
      IR = 0;
      SET_B();
      RES = true;
    }
    PC++;
  }

  set_read();

  uint8_t OP  = IR >> 4;
  uint8_t aaa = (OP & 0b11100000) >> 5;
  uint8_t bbb = (OP & 0b00011100) >> 2;
  uint8_t cc  = OP & 0b00000011;

  if ((cc == 0b01) && (OP != 0x89)) {
    if (aaa == 0b000) ORA();
    if (aaa == 0b001) AND();
    if (aaa == 0b011) ADC();
  } else if (OP == 0x00)
    BRK();
  else if (OP == 0x18)
    CLC();
  else if (OP == 0xA1 || OP == 0xA5 || OP == 0xA9 || OP == 0xAD || OP == 0xB1 || OP == 0xB5 || OP == 0xB9 || OP == 0xBD)
    LDA();
  else if (OP == 0xA4 || OP == 0xA0 || OP == 0xAC || OP == 0xB4 || OP == 0xBC)
    LDX();
  else {
    std::stringstream stream;
    stream << "Attempted to execute opcode 0x" << std::setw(2) << std::setfill('0') << std::hex << (int)(IR >> 4)
           << " at location " << int_to_hex(get_addr());
    throw std::invalid_argument(stream.str());
  }
  IR++;
  ticks++; // increment ticks
}