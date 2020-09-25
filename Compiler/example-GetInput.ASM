%include "vm.inc"

start:
    dw 0x6d56
; PRINT "PASS"
    movd r1, data1
    movd r3, 0x0
    movd r4, 0x3
JLL1:
    mov r3, r1
    push r3
    poc
    advrd r1, 0x1
    movmrb r3, r1
    cmp r3, r4
    jz JLL2
    jnz JLL1
JLL2:
    tib
; Print user input
    movd r0, 0x0
    movd r1, 0x0
JLL4:
    gic r0
    cmp r7, r4
    jz JLL3
    mov r1, r7
    push r1
    pic
    advrd r0, 0x01
    jnz JLL4
JLL3:
    ee

data1:
    db "WRITE SOMETHING: ", 0x3, 0