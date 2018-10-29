#include <stdint.h>

#define MU_IO           0x3F215040
#define MU_LSR          0x3F215054
#define MU_LSR_TX_IDLE  (1U << 6)
#define MU_LSR_TX_EMPTY (1U << 5)

///
// Memory Mapped I/O

void mmio_write(intptr_t reg, uint32_t data){
    *(volatile uint32_t *)reg = data;
}

uint32_t mmio_read(intptr_t reg){
    return *(volatile uint32_t *)reg;
}

///
// Main Function

int main(void){
    while (1) {
        while (!(mmio_read(MU_LSR) & MU_LSR_TX_IDLE) &&
               !(mmio_read(MU_LSR) & MU_LSR_TX_EMPTY))
            ;
        mmio_write(MU_IO, (uint32_t)'A');
    }
    return 0;
}
