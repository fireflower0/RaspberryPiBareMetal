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
// Main Function

int main(void){
    while (1) {
        while (!(MU_LSR & MU_LSR_TX_IDLE) && !(MU_LSR & MU_LSR_TX_EMPTY)){
            asm volatile("nop");
        }
        MU_IO = (unsigned int)'A';
    }
    return 0;
}
