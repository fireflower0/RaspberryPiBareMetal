#include "uart.h"

void put_char(unsigned char ch){
    while(mmio_read(UARTFR) & UARTFR_TXFF);
    mmio_write(UARTDR, ch);
}

void put_char_mini_uart(unsigned char ch){
    while (!(mmio_read(MU_LSR) & MU_LSR_TX_IDLE) &&
           !(mmio_read(MU_LSR) & MU_LSR_TX_EMPTY))
        ;
    mmio_write(MU_IO, ch);
}

void put_str(char *str){
    while(*str != '\0'){
        //put_char(*str++);
        put_char_mini_uart(*str++);
    }
}

void put_hex(uint64_t num){
    int      n    = 0;
    uint64_t base = 16;
    uint64_t d    = 1;
    char buf[32], *bf;

    bf = buf;
    *bf++ = '0';
    *bf++ = 'x';

    while(num / d >= base){
        d *= base;
    }

    while(d != 0){
        int dgt = num / d;
        num %= d;
        d /= base;
        if(n || dgt > 0 || d == 0){
            *bf++ = dgt + (dgt < 10 ? '0' : ('A') - 10);
            ++n;
        }
    }

    *bf++ = '\r';
    *bf++ = '\n';
    *bf = 0;

    put_str(buf);
}
