#include <stdint.h>

#define UARTDR      0x09000000
#define UARTFR      0x09000018
#define UARTFR_TXFF (1U << 5)

///
// Memory Mapped I/O

void mmio_write(intptr_t reg, uint32_t data){
    *(volatile uint32_t *)reg = data;
}

uint32_t mmio_read(intptr_t reg){
    return *(volatile uint32_t *)reg;
}

///
// UART

void put_char(char ch){
    while(mmio_read(UARTFR) & UARTFR_TXFF);
    mmio_write(UARTDR, (uint32_t)ch);
}

void put_str(char *str){
    while(*str != '\0')
        put_char(*str++);
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

///
// Main Function

int main(void){
    put_str("Hello, world!\r\n");
    put_hex(10);

    while(1)
        ;

    return 0;
}
