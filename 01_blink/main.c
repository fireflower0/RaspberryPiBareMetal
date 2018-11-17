#define GPFSEL1 0x3F200004
#define GPSET0  0x3F20001C
#define GPCLR0  0x3F200028
#define MAX_CNT 3000000

///
// Memory Mapped I/O

void mmio_write(unsigned int reg, unsigned int data){
    *(volatile unsigned int *)reg = data;
}

unsigned int mmio_read(unsigned int reg){
    return *(volatile unsigned int *)reg;
}

///
// Main Function

int main(void){
    volatile unsigned int i;

    mmio_write(GPFSEL1, 0x01 << (3 * 6));

    while(1){
        mmio_write(GPSET0, 0x01 << 16);
        for(i = 0; i < MAX_CNT; i++);
        mmio_write(GPCLR0, 0x01 << 16);
        for(i = 0; i < MAX_CNT; i++);
    }

    return 0;
}
