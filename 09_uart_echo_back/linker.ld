SECTIONS
{
    . = 0x80000;

    /* .text : プログラムコードが入る領域 */
    .text : {
        KEEP(*(.text.boot))
        *(.text .text.* .gnu.linkonce.t*)
    }

    /* .rodata : 変更されない文字列とか定数などが入る領域 */
    .rodata : {
        *(.rodata .rodata.* .gnu.linkonce.r*)
    }

    PROVIDE(_data = .);

    /* .data : 初期値を持った変数などを置く領域 */
    .data : {
        *(.data .data.* .gnu.linkonce.d*)
    }

    /* .bss : 初期値を持たない変数などを置く領域 */
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }

    _end = .;

    /* 入力セクション破棄 */
    /DISCARD/ : {
        *(.comment)
        *(.gnu*)
        *(.note*)
        *(.eh_frame*)
    }
}

/* .bssのサイズ */
__bss_size = (__bss_end - __bss_start) >> 3;
