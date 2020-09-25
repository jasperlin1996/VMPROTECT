%include "vm.inc"

start:
    dw 0x6d56
    movd r0, 0x5
    advrd r0, 0x5
    push r0
    pxvn
    ee