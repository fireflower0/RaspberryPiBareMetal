#ifndef __UART_H__
#define __UART_H__

#include "mmio.h"

// PL011
#define UARTDR      0x09000000
#define UARTFR      0x09000018
#define UARTFR_TXFF (1U << 5)

// Mini UART
#define MU_IO           0x3F215040
#define MU_LSR          0x3F215054
#define MU_LSR_TX_IDLE  (1U << 6)
#define MU_LSR_TX_EMPTY (1U << 5)

// PL011
void put_char(unsigned char ch);
void put_str(char *str);
void put_hex(uint64_t num);

// Mini UART
void put_char_mini_uart(unsigned char ch);

#endif // __UART_H__
