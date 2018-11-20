// RaspberryPi3 Memory Mapped I/O Base Address
#define MMIO_BASE 0x3F000000

// Memory Mapped I/O
#define IOREG(X)  (*(volatile unsigned int *) (X))

#define UARTDR      IOREG(0x09000000)
#define UARTFR      IOREG(0x09000018)
#define UARTFR_TXFF (1U << 5)

///
// UART

void put_char(char ch){
    while(UARTFR & UARTFR_TXFF){
        asm volatile("nop");
    }
    UARTDR = ch;
}

void put_str(char *str){
    while(*str != '\0')
        put_char(*str++);
}

void put_hex(unsigned int num){
    unsigned int base = 16;
    unsigned int d    = 1;
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
        if(dgt > 0 || d == 0){
            *bf++ = dgt + (dgt < 10 ? '0' : ('A') - 10);
        }
    }

    // 最後は改行する
    *bf++ = '\r';
    *bf++ = '\n';
    *bf   = 0;

    put_str(buf);
}

///
// Main Function

int main(void){
    put_str("Hello, world!\r\n");
    put_hex(2882400018);

    while(1){
        asm volatile("nop");
    }

    return 0;
}
