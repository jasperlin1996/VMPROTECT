# CPU's opcodes

OPCODE | Mnemonic and params | Description
--- | --- | ---
00  | NOP | No operation |
EE  | EE | End of code and end of the VM's cpu |
01  | MOV r<sub>dst</sub>, r<sub>src</sub> | Move from a register to a register|
02  |  MOVMB r<sub>dst</sub>, addr<sub>src</sub> | Move and extend byte from memory to a register|
03  |  MOVMW r<sub>dst</sub>, addr<sub>src</sub> | Move word from memory to a register |
04  |  MOVB r<sub>dst</sub>, byte | Move and extend byte to a register  |
05  |  MOVW r<sub>dst</sub>, word | Move word to a register |
06  |  MOVBM addr<sub>dst</sub>, r<sub>src</sub> | Move byte from a register to memory location |
07  |  MOVWM addr<sub>dst</sub>, r<sub>src</sub> | Move word from a register to memory location |
08  |  MOVMRB r<sub>dst</sub>, r<sub>src</sub> | Move and extend byte from memory to a register; get an address from a register |
09  |  MOVMRW r<sub>dst</sub>, r<sub>src</sub> | Move word from memory to a register; get an address from a register |
0A  |  MOVMD r<sub>dst</sub>, addr<sub>src</sub> | Move double word from memory to a register |
0B  |  MOVD r<sub>dst</sub>, dword | Move double word to a register |
0C  |  MOVDM addr<sub>dst</sub>, r<sub>src</sub> | Move double word from a register to memory location |
0D  |  MOVMRD r<sub>dst</sub>, r<sub>src</sub> | Move double from memory to a register; get an address from a register |
  | | |
20  |  JMP addr | Unconditional jump |
21  |  JZ addr | Jump if equal; it set up PC to the specified location if the ZF is set (1) |
22  |  JNZ addr | Jump if not equal; it set up PC to the specified location if the ZF is not set (0) |
23  |  JAE addr | Jump if above or equal; it set up PC to the specified location if the ZF is set (1) and the CF is not set (0) |
24  |  JBE addr | Jump if below or equal; it set up PC to the specified location if the ZF is set (1) and the CF is set (1) |
25  |  JB addr | Jump if below; it set up PC to the specified location if the ZF is not set (0) and the CF is set (1) |
26  |  JA addr | Jump if above; it set up PC to the specified location if the ZF is not set (0) and the CF is not set (0) |
  | | |
30  |  ADVR r<sub>dst</sub>, word | Add word value to a register |
31  |  ADRR r<sub>dst</sub>, r<sub>src</sub> | Add two registers |
32  |  ADRRL r<sub>dst</sub>, r<sub>src</sub> | Add two registers (the low byte) |
33  |  SUBVR r<sub>dst</sub>, word | Substract word value from a register |
34  |  SUBRR r<sub>dst</sub>, r<sub>src</sub> | Substract two registers |
35  |  SUBRRL r<sub>dst</sub>, r<sub>src</sub> | Substract two registers (the low byte) |
36  |  XOR r<sub>dst</sub>, r<sub>src</sub> | Xor two registers |
37  |  XOR r<sub>dst</sub>, r<sub>src</sub> | Xor two registers (the low byte) |
38  |  NOT r<sub>dst</sub>| Bitwise NOT on value in a register |
39  |  NOT r<sub>dst</sub> | Bitwise NOT on value in a register (the low byte) |
3A  |  ADVRD r<sub>dst</sub>, dword | Add double word value to a register |
3B  |  SUBVR r<sub>dst</sub>, dword | Substract double word value from a register |
3C  |  SHR r<sub>dst</sub>, count<sub>byte</sub> | Shift the bits of the operand destination to the right, by the number of bits specified in the count operand |
3D  |  SHL r<sub>dst</sub>, count<sub>byte</sub> | Shift the bits of the operand destination to the left, by the number of bits specified in the count operand |
  | | |
50  |  CMP r<sub>dst</sub>, r<sub>src</sub> | Compare two registers |
51  |  CMPL r<sub>dst</sub>, r<sub>src</sub> | Compare two registers (the low byte) |
  | | |
60  |  VMSYSBUS word | Arguments to functions pass via the stack |
  | | |
90  |  PUSH r<sub>src</sub> | Push value from a register to stack |
91  |  POP r<sub>dst</sub> | Pop value from stack to a register |
92  |  CLST | Clear the stack |
93  |  SETSP dword| Set the stack pointer to the double word value |
  | | |
A0  |  POC  | Print char without new line, the value must be at the top of the stack |
A1  |  POCN  | Print char with new line, the value must be at the top of the stack |
A2  |  TIB  | Take input and move to the data buffer, the length of the string is stored in R[7] |
A3  |  GIC r<sub>src</sub> | Get a specific char from input, that is stored in the data buffer, the value will be stored in R[7], pass the position of char via a some register |
A4  |  PIC  | Print char from input without new line, the value must be at the top of the stack |
A5  |  PICN  | Print char from input with new line, the value must be at the top of the stack |
A6  |  PXV  | Print a value in hex, the value must be at the top of the stack |
A7  |  PXVN  | Print a value in hex with a new line, the value must be at the top of the stack |