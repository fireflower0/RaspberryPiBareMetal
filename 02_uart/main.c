#include <stdint.h>

#define UARTDR      (*(volatile unsigned int *)0x09000000)
#define UARTFR      (*(volatile unsigned int *)0x09000018)
#define UARTFR_TXFF (1U << 5)

void put_char(char ch){
    while(UARTFR & UARTFR_TXFF);
    UARTDR = (unsigned int)ch;
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

    while(d != 0){
        int dgt = num / d;
        num %= d;
        d /= base;
        if(n || dgt > 0 || d == 0){
            *bf++ = dgt + (dgt < 10 ? '0' : ('A') - 10);
            ++n;
        }
    }
    *bf++ = '\n';
    *bf = 0;

    put_str(buf);
}

int main(void){
    put_str("Hello, world!\n");
    put_hex(10);

    while(1)
        ;

    return 0;
}
