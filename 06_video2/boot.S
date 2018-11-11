    .section ".text.boot"

    .global _start

_start:
    // Read CPU ID, Stop slave cores
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, 2f
1:
    // CPU ID > 0, STOP
    wfe
    b   1b
2:
    // CPU ID == 0

    // Set stack before our code
    ldr x1, =_start
    mov sp, x1

    // Clear bss
    ldr x1, =__bss_start
    ldr w2, =__bss_size
3:
    cbz w2, 4f
    str xzr, [x1], #8
    sub w2, w2, #1
    cbnz w2, 3b
4:
    // Jump to C code, should not return
    bl  main
    // for failsafe, halt this core too
    b   1b
