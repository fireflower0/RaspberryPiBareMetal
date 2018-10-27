#define GPFSEL1 0x3F200004
#define GPSET0  0x3F20001C
#define GPCLR0  0x3F200028
#define MAX_CNT 3000000

int main(void){
    volatile unsigned int i;

    *(volatile unsigned int *)GPFSEL1 = 0x01 << (3 * 6);

    while(1){
        *(volatile unsigned int *)GPSET0 = 0x01 << 16;
        for(i = 0; i < MAX_CNT; i++);
        *(volatile unsigned int *)GPCLR0 = 0x01 << 16;
        for(i = 0; i < MAX_CNT; i++);
    }

    return 0;
}
