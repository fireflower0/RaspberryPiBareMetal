#include <stdio.h>
#include <stdint.h>
#include "mmio.h"
#include "uart.h"

// Mail Box
#define MB_RECV_MAIL   0x3F00B880
#define MB_RECV_STATUS 0x3F00B898
#define MB_SEND_MAIL   0x3F00B8A0
#define MB_SEND_STATUS 0x3F00B8B8

#define MAIL_FULL  0x80000000
#define MAIL_EMPTY 0x40000000

///
// Mail Box

void mbox_write(uint8_t channel, intptr_t msg){
    uint32_t sta;

    put_str("mbox_write: Start!\r\n");

    if((msg & 0xFU) == 0){
        do{
            put_str("mb_send_status: ");
            sta = mmio_read(MB_SEND_STATUS);
            put_hex(sta);
        }while((sta & MAIL_FULL) != 0);

        mmio_write(MB_SEND_MAIL, msg | channel);
    }

    put_str("mbox_write: End!\r\n");
}

uint32_t mbox_read(uint8_t channel){
    uint32_t data, sta;

    put_str("mbox_read: Start!\r\n");

    do{
        do{
            put_str("mb_recv_status: ");
            sta = mmio_read(MB_RECV_STATUS);
            put_hex(sta);
        }while(sta & MAIL_EMPTY);
        data = mmio_read(MB_RECV_MAIL);
        put_hex(data);
    }while((data & 0xFU) != channel);

    put_str("mbox_read: End!\r\n");

    return data >> 4;
}

void fb_init() {
    uint32_t message[] __attribute__((aligned(16))) = {
        32,  // The whole buffer is 32 bytes
        0,   // This is a request, so the request/response code is 0
        // This tag requests a 16 byte aligned framebuffer
        0x00040001, 8, 0, 16, 0,
        0    // This is the end tag
    };

    put_str("fb_init: start!\r\n");

    put_str("Message Address: ");
    put_hex((intptr_t)message);
    for (int i = 0; i < 8; i++){
        put_hex(message[i]);
    }

    put_str("Writing Mailbox.\r\n");

    //mbox_write(8, (uint32_t)message + 0x40000000);
    mbox_write(8, (intptr_t)message);

    put_str("done.\r\n");

    put_str("Reading Mailbox.\r\n");

    mbox_read(8);

    put_str("done.\r\n");

    put_str("Message Address: ");
    put_hex((intptr_t)message);
    for (int i = 0; i < 8; i++){
        put_hex(message[i]);
    }
}

///
// Main Function

int main(void){
    fb_init();

    while(1)
        ;

    return 0;
}
