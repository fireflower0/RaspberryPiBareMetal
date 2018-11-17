// RaspberryPi3 Memory Mapped I/O Base Address
#define MMIO_BASE 0x3F000000

// Memory Mapped I/O
#define IOREG(X)  (*(volatile unsigned int *) (X))

// GPIO16 control register
#define GPFSEL1 IOREG(MMIO_BASE + 0x00200004)  // GPIO Function Select
#define GPSET0  IOREG(MMIO_BASE + 0x0020001C)  // GPIO16 High
#define GPCLR0  IOREG(MMIO_BASE + 0x00200028)  // GPIO16 Low

// Sleep count
#define MAX_CNT 3000000

///
// Main Function

int main(void){
    volatile unsigned int i;

    // Set the function assigned to the pin to GPIO
    GPFSEL1 = 0x01 << 18;

    while(1){
        GPSET0 = 0x01 << 16;           // GPIO16 High
        for(i = 0; i < MAX_CNT; i++);  // Sleep
        GPCLR0 = 0x01 << 16;           // GPIO16 Low
        for(i = 0; i < MAX_CNT; i++);  // Sleep
    }

    return 0;
}
