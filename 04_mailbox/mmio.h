#ifndef __MMIO_H__
#define __MMIO_H__

#include <stdint.h>

void mmio_write(intptr_t reg, uint32_t data);
uint32_t mmio_read(intptr_t reg);

#endif // __MMIO_H__
