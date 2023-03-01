NMI_VECTOR = 0x02
START = $0200

VAL: .word 0x0503

  .org START
  CLC
  ADC #0x01
  ADC 0x00
  ADC VAL+1
  ADC VAL,X
  ADC 0x00,X
  ADC VAL,Y
  ADC (VAL,X)
  ADC (VAL),Y
  STP

  .org 0x0503
  .byte 0x06

  .org 0xFFFA
  .word NMI_VECTOR
  .word START
  .word 0x0300