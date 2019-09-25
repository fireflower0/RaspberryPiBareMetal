    .section ".text.boot"

    .global _start

_start:
    // CPU IDの読み取り、スレーブコアの停止
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, 2f
1:
    // CPU ID > 0, STOP
    wfe
    b   1b
2:
    // CPU ID == 0

    // コードの前にスタックを設定する
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
    // C言語プログラムのメイン関数にジャンプ
    bl  main
    // フェイルセーフの場合は、このコアも停止
    b   1b
