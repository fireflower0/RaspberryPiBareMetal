// RaspberryPi3 Memory Mapped I/O Base Address
#define MMIO_BASE 0x3F000000

// Memory Mapped I/O
#define IOREG(X)  (*(volatile unsigned int *) (X))

// Mail Box
#define MB_RECV_MAIL   IOREG(MMIO_BASE + 0x0000B880)
#define MB_RECV_STATUS IOREG(MMIO_BASE + 0x0000B898)
#define MB_SEND_MAIL   IOREG(MMIO_BASE + 0x0000B8A0)
#define MB_SEND_STATUS IOREG(MMIO_BASE + 0x0000B8B8)

#define MAIL_FULL      0x80000000
#define MAIL_EMPTY     0x40000000

// UART
#define MU_IO           IOREG(MMIO_BASE + 0x00215040)
#define MU_LSR          IOREG(MMIO_BASE + 0x00215054)
#define MU_LSR_TX_IDLE  (1U << 6)
#define MU_LSR_TX_EMPTY (1U << 5)

///
// UART

// 文字
void put_char(unsigned char ch){
    // UARTの送信FIFOの状態を確認
    while (!(MU_LSR & MU_LSR_TX_IDLE) && !(MU_LSR & MU_LSR_TX_EMPTY)){
        asm volatile("nop");
    }
    // UARTの送信FIFOへ1バイト追加
    MU_IO = ch;
}

// 文字列
void put_str(char *str){
    while(*str != '\0'){
        put_char(*str++);
    }
}

// 16進数
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
// Mail Box

void mbox_write(unsigned char channel, unsigned int msg){
    unsigned int sta;

    if((msg & 0xFU) == 0){
        do{
            sta = MB_SEND_STATUS;
        }while((sta & MAIL_FULL) != 0);

        MB_SEND_MAIL = msg | channel;
    }
}

unsigned int mbox_read(unsigned char channel){
    unsigned int data, sta;

    do{
        do{
            sta = MB_RECV_STATUS;
        }while(sta & MAIL_EMPTY);
        data = MB_RECV_MAIL;
    }while((data & 0xFU) != channel);

    return data >> 4;
}

void fb_init() {
    unsigned int message[] __attribute__((aligned(16))) = {
        32,  // The whole buffer is 32 bytes
        0,   // This is a request, so the request/response code is 0
        // This tag requests a 16 byte aligned framebuffer
        0x00040001, 8, 0, 16, 0,
        0    // This is the end tag
    };

    put_str("fb_init: start!\r\n");

    put_str("*****Write Data*****\r\n");
    for (int i = 0; i < 8; i++){
        put_hex(message[i]);
        put_str("\r\n");
    }

    put_str("Writing Mailbox.\r\n");

    mbox_write(8, (unsigned long long)message + 0x40000000);

    put_str("done.\r\n");

    put_str("Reading Mailbox.\r\n");

    mbox_read(8);

    put_str("done.\r\n");

    put_str("*****RESULT*****\r\n");
    for (int i = 0; i < 8; i++){
        put_hex(message[i]);
        put_str("\r\n");
    }
}

///
// Main Function

int main(void){
    fb_init();

    while(1)
        ;

    return 0;
}
