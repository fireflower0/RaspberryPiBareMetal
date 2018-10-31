#include "mmio.h"

void mmio_write(intptr_t reg, uint32_t data){
    *(volatile uint32_t *)reg = data;
}

uint32_t mmio_read(intptr_t reg){
    return *(volatile uint32_t *)reg;
}
