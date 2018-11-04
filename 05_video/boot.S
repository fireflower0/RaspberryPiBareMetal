    .global _start
_start:
    mov x0, #0x8000
    mov sp, x0
    bl  main
