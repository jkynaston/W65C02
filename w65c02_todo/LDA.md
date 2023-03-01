Load accumulator with memory
M -> A (Nv-bdiZc)

|done?|addressing|assembly|opcode|bytes|cycles|
|---|---|---|---|---|---|
||immediate    |LDA \#oper   |0xA9|2|2 |
||zeropage     |LDA oper     |0xA5|2|3 |
||zeropage,X   |LDA oper,X   |0xB5|2|4 |
||absolute     |LDA oper     |0xAD|3|4 |
||absolute,X   |LDA oper,X   |0xBD|3|4*|
||absolute,Y   |LDA oper,Y   |0xB9|3|4*|
||(indirect),X |LDA (oper,X) |0xA1|2|6 |
||(indirect),Y |LDA (oper,Y) |0xB1|2|5*|