    .global _start
_start:
    mov x0, #0x80000
    mov sp, x0
    bl  main
