#ifndef __MAILBOX_H__
#define __MAILBOX_H__

#include <stdint.h>

#include "mmio.h"
#include "uart.h"

#define MB_RECV_MAIL   0x3F00B880
#define MB_RECV_STATUS 0x3F00B898
#define MB_SEND_MAIL   0x3F00B8A0
#define MB_SEND_STATUS 0x3F00B8B8

#define MAIL_FULL  0x80000000
#define MAIL_EMPTY 0x40000000

typedef struct _fb_info_t {
    uint32_t display_w;  // display width
    uint32_t display_h;  // display height
    uint32_t w;          // framebuffer width
    uint32_t h;          // framebuffer height
    uint32_t row_bytes;  // write 0 to get value
    uint32_t bpp;        // bits per pixel
    uint32_t ofs_x;      // x offset of framebuffer
    uint32_t ofs_y;      // y offset of framebuffer
    uint32_t buf_addr;   // pointer to framebuffer
    uint32_t buf_size;   // framebuffer size in bytes
} fb_info_t;

void     mbox_write(uint8_t channel, intptr_t msg);
uint32_t mbox_read(uint8_t channel);
void     fb_init(fb_info_t *fb_info);

#endif // __MAILBOX_H__
