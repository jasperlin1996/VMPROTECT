# VMPROTECT
A code obfuscation method using virtual machines to protect a product. 

A virtual machine simulates a CPU along with a few other hardware components, allowing it to perform arithmetic, read and write to memory and interact with I/O devices. It can understand a machine language which you can use to program it. Virtual machines used in code obfuscation are completely different than common virtual machnines. They are very specific to the task of executing a few set of instructions. Each instruction is given a custom opcode (often generated at random).

## Documentation
The VM will simulate a fictional cpu (32-bit). It has a custom instrucion set compared to x86-64.

### Memory
The VM has 51,200 memory locations, each of which stores a 8-bit value (it can store a total of 50kb). The VM has stack, which is a separate data structure. The stack has 256 memory locations, each of which stores a 32-bit value (it can store a total of 512b).

```c++
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

BYTE codeData[51200];
DWORD stack[256];
```

### Registers
A register is a slot for storing value on the CPU. The VM has 10 total registers, each of which is 4 bytes (32 bits). The six of them are general purpose, one has designated role as program counter and another has role as stack pointer. The VM has also two regisers ZF (Zero Flag) and CF (Carry Flag). These two provide information about the most recently executed calculation (allows to check logical conditions such as *AND*).

```c++
/* General Purpose Registers R0 -> R7 */
DWORD R[8];
struct {
    /* Zero Flag 
        value 1 - flag is set if the result of the last comparison was zero
        value 0 - flag is not set
    */
    unsigned char ZF : 1;
    /* Carry Flag 
        value 1 - flag is set the results of the last comparison was moving
        value 0 - flag is not set
    */
    unsigned char CF : 1;
};
/* Program Counter */
DWORD PC;
/* Stack Pointer */
DWORD SP;
```

### Instructions
An instruction is a command which tells the CPU (and the VM's cpu) to do some task, such compare two values. Instructions have both an opcode which indicates the kind of task to perform and a set of parameters which provide inputs to the task being performed.

```assembly
ADRR R2, R1 => 22 02 01
```

<details>
  <summary>Show full list of opcodes</summary>

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
0B  |  MOVD r<sub>dst</sub>, word | Move double word to a register |
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
  | | |
50  |  CMP r<sub>dst</sub>, r<sub>src</sub> | Compare two registers |
51  |  CMPL r<sub>dst</sub>, r<sub>src</sub> | Compare two registers (the low byte) |
  | | |
90  |  PUSH r<sub>src</sub> | Push value from a register to stack |
91  |  POP r<sub>dst</sub> | Pop value from stack to a register |
  | | |
A0  |  POC  | Print char without new line, the value must be at the top of the stack |
A1  |  POCN  | Print char with new line, the value must be at the top of the stack |

</details>
