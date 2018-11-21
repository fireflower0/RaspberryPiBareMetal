// RaspberryPi3 Memory Mapped I/O Base Address
#define MMIO_BASE 0x3F000000

// Memory Mapped I/O
#define IOREG(X)  (*(volatile unsigned int *) (X))

// UART
#define MU_IO           IOREG(MMIO_BASE + 0x00215040)
#define MU_LSR          IOREG(MMIO_BASE + 0x00215054)
#define MU_LSR_TX_IDLE  (1U << 6)
#define MU_LSR_TX_EMPTY (1U << 5)

///
// UART Function

void put_char(unsigned char ch){
    while (!(MU_LSR & MU_LSR_TX_IDLE) && !(MU_LSR & MU_LSR_TX_EMPTY)){
        asm volatile("nop");
    }
    MU_IO = ch;
}

void put_str(char *str){
    while(*str != '\0'){
        put_char(*str++);
    }
}

void put_hex(unsigned long long num){
    int      n    = 0;
    unsigned long long base = 16;
    unsigned long long d    = 1;
    char buf[32], *bf;

    bf = buf;

    // 先頭に"0x"を付ける
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

    *bf   = 0;

    put_str(buf);
}

///
// Main Function

void main(void){
    put_char('A');
    put_str("\r\nHello, world!\r\n");
    put_hex(2882400018);
    put_str("\r\n");

    while (1) {
        asm volatile("nop");
    }
}
