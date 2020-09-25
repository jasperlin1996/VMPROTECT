; pass: vmobfuscationisthebest

%include "vm.inc"

start:
    dw 0x6d56
    movd r0, 0x0
    movd r1, data3
    movd r2, data3
    movd r6, 0x3
GETIN1:
    mov r3, r1
    push r3
    poc
    advrd r1, 0x1
    advrd r2, 0x1
    movmrb r3, r2
    cmp r3, r6
    jz GETIN2
    jnz GETIN1
GETIN2:
    tib
    movd r3, 0x10
    movd r1, 0x1
    movd r2, 0x0
    movd r4, 0x0
    movd r5, 0x3
    advrd r3, 0x5
JLL9:
    gic r1
    mov r2, r7
    cmp r2, r5
    jz JLL8
    advrd r1, 0x1
    adrr r4, r2
    cmp r2, r5
    advrd r4, 0x5

    gic r1
    mov r2, r7
    cmp r2, r5
    jz JLL8
    advrd r1, 0x1
    adrr r4, r2
    advrd r4, 0xa

    gic r1
    mov r2, r7
    cmp r2, r5
    jz JLL8
    advrd r1, 0x1
    adrr r4, r2
    subvrd r4, 0x6

    gic r1
    mov r2, r7
    cmp r2, r5
    jz JLL8
    advrd r1, 0x1
    mov r2, r7
    adrr r4, r2
    subvrd r4, 0xc

    gic r1
    mov r2, r7
    cmp r2, r5
    jz JLL8
    advrd r1, 0x1
    adrr r4, r2
    advrd r4, 0xf

    jnz JLL9

JLL8:    
    advrd r3, 0x5
    movd r3, 0x8a
    movd r5, 0x980
    xor r4, r5
    cmp r4, r3
    jz JLL6
    jnz JLL7

JLL6:
; PRINT "PASS"
    movd r4, 0x0
    jmp JLL5

JLL7:
; PRINT "FAILED"
    movd r4, 0x0
    jmp JLL4

JLL5:
    movd r1, data1
    jmp JLL3

JLL4:
    movd r1, data2
    jmp JLL3

JLL3:
    mov r2, r1
    advrd r2, 0x1
    movd r3, 0x0
    movd r4, 0x0

JLL1:
    mov r3, r1
    push r3
    poc
    advrd r1, 0x1
    advrd r2, 0x1
    movmrb r3, r2
    cmp r3, r4
    jz JLL2
    jnz JLL1

JLL2:
    mov r3, r1
    push r3
    pocn
    ee

data1:
    db "PASS", 0 
data2:
    db "FAILED", 0
data3:
    db "PASSWORD: ", 0x3, 0