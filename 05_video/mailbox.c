#include "mailbox.h"

void mbox_write(uint8_t channel, intptr_t msg){
    uint32_t sta;

    if((msg & 0xFU) == 0){
        do{
            sta = mmio_read(MB_SEND_STATUS);
        }while((sta & MAIL_FULL) != 0);

        mmio_write(MB_SEND_MAIL, msg | channel);
    }
}

uint32_t mbox_read(uint8_t channel){
    uint32_t data, sta;

    do{
        do{
            sta = mmio_read(MB_RECV_STATUS);
        }while(sta & MAIL_EMPTY);
        data = mmio_read(MB_RECV_MAIL);
    }while((data & 0xFU) != channel);

    return data >> 4;
}

void fb_init(fb_info_t *fb_info) {
    uint32_t message[] __attribute__((aligned(16))) = {
        112,                        // buffer is 112 bytes
        0,                          // This is a request
        0x00048003, 8, 0,           // Set the screen size to..
        fb_info->display_w,         // @5
        fb_info->display_h,         // @6
        0x00048004, 8, 0,           // Set the virtual screen size to..
        fb_info->w,                 // @10
        fb_info->h,                 // @11
        0x00048005, 4, 0,           // Set the depth to..
        fb_info->bpp,               // @15
        0x00040008, 4, 0,           // Get the pitch
        0,                          // @19
        0x00040001, 8, 0,           // Get the frame buffer address..
        16, 0,                      // @23 a 16 byte aligned
        0,                          // @25 the end tag
        0, 0,                       // padding; 16 byte aligned
    };

    put_str("fb_init: start!\r\n");

    put_str("Message Address: ");
    put_hex((intptr_t)message);
    for (int i = 0; i < 28; i++){
        put_hex(message[i]);
    }

    put_str("Writing Mailbox.\r\n");

    mbox_write(8, (uintptr_t)message + 0x40000000);
    //mbox_write(8, (uintptr_t)message);

    put_str("done.\r\n");

    put_str("Reading Mailbox.\r\n");

    mbox_read(8);

    put_str("done.\r\n");

    put_str("Message Address: ");
    put_hex((intptr_t)message);
    for (int i = 0; i < 28; i++){
        put_hex(message[i]);
    }

    fb_info->display_w = message[5];
    fb_info->display_h = message[6];
    fb_info->w = message[10];
    fb_info->h = message[11];
    fb_info->row_bytes = message[19];
    fb_info->bpp = message[15];
    fb_info->buf_addr = message[23] & 0x3ffffffff;
    fb_info->buf_size = message[24];
}
