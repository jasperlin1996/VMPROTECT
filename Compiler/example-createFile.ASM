%include "vm.inc"

start:
    dw 0x6d56
    movd r1, 0x0
    movd r2, 0x0
    movd r1, path
    movd r2, data
    push r1
    push r2
    vmsysbus sysfilecr
    ee

data:
    db 0x01, 0x02, 0x03, 0x04, 0x3, 0xD
path:
    db "/home/eaglx/file.bin", 0x3, 0xD