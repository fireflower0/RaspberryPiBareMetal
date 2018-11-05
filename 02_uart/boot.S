    .global _start
_start:
    //mov x0, #0x8000
    mov x0, #0x40080000
    mov sp, x0
    bl  main
