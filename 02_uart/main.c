// RaspberryPi3 Memory Mapped I/O Base Address
#define MMIO_BASE 0x3F000000

// Memory Mapped I/O
#define IOREG(X)  (*(volatile unsigned int *) (X))

// Mailbox
volatile unsigned int __attribute__((aligned(16))) mbox[36];

#define VIDEOCORE_MBOX  (MMIO_BASE+0x0000B880)
#define MBOX_READ       IOREG(VIDEOCORE_MBOX + 0x00)
#define MBOX_POLL       IOREG(VIDEOCORE_MBOX + 0x10)
#define MBOX_SENDER     IOREG(VIDEOCORE_MBOX + 0x14)
#define MBOX_STATUS     IOREG(VIDEOCORE_MBOX + 0x18)
#define MBOX_CONFIG     IOREG(VIDEOCORE_MBOX + 0x1C)
#define MBOX_WRITE      IOREG(VIDEOCORE_MBOX + 0x20)

#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000

#define MBOX_REQUEST    0

/* channels */
#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8

/* tags */
#define MBOX_TAG_GETSERIAL      0x10004
#define MBOX_TAG_SETCLKRATE     0x38002
#define MBOX_TAG_LAST           0

// UART
#define PL011_DR        IOREG(MMIO_BASE+0x00201000)
#define PL011_FR        IOREG(MMIO_BASE+0x00201018)
#define PL011_IBRD      IOREG(MMIO_BASE+0x00201024)
#define PL011_FBRD      IOREG(MMIO_BASE+0x00201028)
#define PL011_LCRH      IOREG(MMIO_BASE+0x0020102C)
#define PL011_CR        IOREG(MMIO_BASE+0x00201030)
#define PL011_IMSC      IOREG(MMIO_BASE+0x00201038)
#define PL011_ICR       IOREG(MMIO_BASE+0x00201044)

// GPIO
#define GPFSEL1         IOREG(MMIO_BASE+0x00200004)
#define GPPUD           IOREG(MMIO_BASE+0x00200094)
#define GPPUDCLK0       IOREG(MMIO_BASE+0x00200098)

///
// Mailbox

int mbox_call(unsigned char ch){
    unsigned int r = (((unsigned int)((unsigned long)&mbox) & ~0xF) | (ch & 0xF));

    /* メールボックスに書き込むまで待つ */
    do{
        asm volatile("nop");
    }while(MBOX_STATUS & MBOX_FULL);

    /* メッセージのアドレスをチャネル識別子を持つメールボックスに書き込む */
    MBOX_WRITE = r;

    while(1) {
        /* 応答を待つ */
        do{
            asm volatile("nop");
        }while(MBOX_STATUS & MBOX_EMPTY);

        /* メッセージに対する応答か判定 */
        if(r == MBOX_READ){
            /* 有効な成功応答か判定 */
            return mbox[1] == MBOX_RESPONSE;
        }
    }
    return 0;
}

///
// UART

void uart_init(){
    register unsigned int r;

    /* initialize UART */
    PL011_CR = 0;         // turn off UART0

    /* set up clock for consistent divisor values */
    mbox[0] = 9 * 4;
    mbox[1] = MBOX_REQUEST;
    mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
    mbox[3] = 12;
    mbox[4] = 8;
    mbox[5] = 2;           // UART clock
    mbox[6] = 4000000;     // 4Mhz
    mbox[7] = 0;           // clear turbo
    mbox[8] = MBOX_TAG_LAST;

    mbox_call(MBOX_CH_PROP);

    /* map UART0 to GPIO pins */
    r = GPFSEL1;
    r &= ~((7 << 12) | (7 << 15)); // gpio14, gpio15
    r |= (4 << 12) | (4 << 15);    // alt0
    GPFSEL1 = r;
    GPPUD = 0;            // enable pins 14 and 15
    r = 150;
    while(r--) {
        asm volatile("nop");
    }
    GPPUDCLK0 = (1 << 14) | (1 << 15);
    r = 150;
    while(r--) {
        asm volatile("nop");
    }
    GPPUDCLK0 = 0;        // flush GPIO setup

    PL011_ICR  = 0x7FF;     // clear interrupts
    PL011_IBRD = 2;         // 115200 baud
    PL011_FBRD = 0xB;
    PL011_LCRH = 0b11<<5;   // 8n1
    PL011_CR   = 0x301;     // enable Tx, Rx, FIFO
}

void put_char(char ch){
    do{
        asm volatile("nop");
    }while(PL011_FR & 0x10);
    PL011_DR = ch;
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

void get_serial_number(){
    // get the board's unique serial number with a mailbox call
    mbox[0] = 8 * 4;                // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = MBOX_TAG_GETSERIAL;   // get serial number command
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    // send the message to the GPU and receive answer
    if (mbox_call(MBOX_CH_PROP)) {
        put_str("My serial number is: ");
        put_hex(mbox[6]);
        put_hex(mbox[5]);
        put_str("\r\n");
    } else {
        put_str("Unable to query serial!\r\n");
    }
}

///
// Main Function

int main(void){
    uart_init();

    get_serial_number();

    while(1){
        asm volatile("nop");
    }

    return 0;
}
