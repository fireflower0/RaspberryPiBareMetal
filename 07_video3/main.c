#define MMIO_BASE       0x3F000000

// Memory Mapped I/O
#define IOREG(X)  (*(volatile unsigned int *) (X))

/* mailbox message buffer */
volatile unsigned int  __attribute__((aligned(16))) mbox[36];

#define VIDEOCORE_MBOX  (MMIO_BASE+0x0000B880)
#define MBOX_READ       IOREG(VIDEOCORE_MBOX + 0x00)
#define MBOX_POLL       IOREG(VIDEOCORE_MBOX + 0x10)
#define MBOX_SENDER     IOREG(VIDEOCORE_MBOX + 0x14)
#define MBOX_STATUS     IOREG(VIDEOCORE_MBOX + 0x18)
#define MBOX_CONFIG     IOREG(VIDEOCORE_MBOX + 0x1C)
#define MBOX_WRITE      IOREG(VIDEOCORE_MBOX + 0x20)

#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000

#define MBOX_REQUEST    0

/* channels */
#define MBOX_CH_PROP    8

/* tags */
#define MBOX_TAG_LAST   0

void mbox_write(unsigned char ch){
    unsigned int r = (((unsigned int)((unsigned long)&mbox) & ~0xF) | (ch & 0xF));

    /* メールボックスに書き込むまで待つ */
    do{
        asm volatile("nop");
    }while(MBOX_STATUS & MBOX_FULL);

    /* メッセージのアドレスをチャネル識別子を持つメールボックスに書き込む */
    MBOX_WRITE = r;
}

int mbox_read(unsigned char ch){
    unsigned int r = (((unsigned int)((unsigned long)&mbox) & ~0xF) | (ch & 0xF));
    while(1) {
        /* 応答を待つ */
        do{
            asm volatile("nop");
        }while(MBOX_STATUS & MBOX_EMPTY);

        /* メッセージに対する応答か判定 */
        if(r == MBOX_READ){
            /* 有効な成功応答か判定 */
            return mbox[1] == MBOX_RESPONSE;
        }
    }
    return 0;
}

typedef struct _fb_info_t {
    unsigned int  display_w;  // display width
    unsigned int  display_h;  // display height
    unsigned int  w;          // framebuffer width
    unsigned int  h;          // framebuffer height
    unsigned int  row_bytes;  // write 0 to get value
    unsigned int  bpp;        // bits per pixel
    unsigned int  ofs_x;      // x offset of framebuffer
    unsigned int  ofs_y;      // y offset of framebuffer
    unsigned char *buf_addr;  // pointer to framebuffer
    unsigned int  buf_size;   // framebuffer size in bytes
} fb_info_t;

void lfb_init(fb_info_t *fb_info){
    for(int i = 0; i < 36; i++){
        mbox[i] = 0;
    }

    mbox[0] = 112;
    mbox[1] = MBOX_REQUEST;

    mbox[2] = 0x48003;  //set phy wh
    mbox[3] = 8;
    mbox[4] = 0;
    mbox[5] = fb_info->display_w;  //FrameBufferInfo.width
    mbox[6] = fb_info->display_h;  //FrameBufferInfo.height

    mbox[7] = 0x48004;  //set virt wh
    mbox[8] = 8;
    mbox[9] = 0;
    mbox[10] = fb_info->w;        //FrameBufferInfo.virtual_width
    mbox[11] = fb_info->h;        //FrameBufferInfo.virtual_height
    
    mbox[12] = 0x48005; //set depth
    mbox[13] = 4;
    mbox[14] = 0;
    mbox[15] = fb_info->bpp;      //FrameBufferInfo.depth

    mbox[16] = 0x40008; //get pitch
    mbox[17] = 4;
    mbox[18] = 0;
    mbox[19] = 0;       //FrameBufferInfo.pitch

    mbox[20] = 0x40001; //get framebuffer, gets alignment on request
    mbox[21] = 8;
    mbox[22] = 0;
    mbox[23] = 0;       //FrameBufferInfo.pointer
    mbox[24] = 0;       //FrameBufferInfo.size

    mbox[25] = MBOX_TAG_LAST;

    mbox_write(MBOX_CH_PROP);

    if(mbox_read(MBOX_CH_PROP) && mbox[15] == 16 && mbox[23] != 0) {
        mbox[23] &= 0x3FFFFFFF;
        fb_info->display_w = mbox[5];
        fb_info->display_h = mbox[6];
        fb_info->bpp       = mbox[15];
        fb_info->row_bytes = mbox[19];
        fb_info->buf_addr  = (void*)((unsigned long)mbox[23]);
    }
}

static inline void *coord2ptr(unsigned char *vram,
                              unsigned int pitch,
                              unsigned int bpp,
                              int x,
                              int y) {
    return (void *)(vram + ((bpp + 7) >> 3) * x + pitch * y);
}

void boxfill8(unsigned char *vram,
              unsigned int c,
              unsigned int pitch,
              unsigned int bpp,
              int x0, int y0, int x1, int y1){
    int x, y;
    unsigned short int *p;

    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++){
            p = (unsigned short int *)coord2ptr(vram, pitch, bpp, x, y);
            *p = c;
        }
    }
}

#define COL8_000000 (((0x00>>3)<<11) + ((0x00>>3)<<6) + (0x00>>3))
#define COL8_008484 (((0x00>>3)<<11) + ((0x84>>3)<<6) + (0x84>>3))
#define COL8_848484 (((0x84>>3)<<11) + ((0x84>>3)<<6) + (0x84>>3))
#define COL8_C6C6C6 (((0xC6>>3)<<11) + ((0xC6>>3)<<6) + (0xC6>>3))
#define COL8_FFFFFF (((0xFF>>3)<<11) + ((0xFF>>3)<<6) + (0xFF>>3))

int main(void){
    fb_info_t fb_info = {1280, 800, 1280, 800, 0, 16, 0, 0, 0, 0};

    lfb_init(&fb_info);

    unsigned char *vram  = fb_info.buf_addr;
    unsigned int  pitch  = fb_info.row_bytes;
    unsigned int  bpp    = fb_info.bpp;
    unsigned int  x = fb_info.display_w;
    unsigned int  y = fb_info.display_h;

    boxfill8(vram, COL8_008484, pitch, bpp,  0,     0,      x -  1, y - 29);
    boxfill8(vram, COL8_C6C6C6, pitch, bpp,  0,     y - 28, x -  1, y - 28);
    boxfill8(vram, COL8_FFFFFF, pitch, bpp,  0,     y - 27, x -  1, y - 27);
    boxfill8(vram, COL8_C6C6C6, pitch, bpp,  0,     y - 26, x -  1, y -  1);

    boxfill8(vram, COL8_FFFFFF, pitch, bpp,  3,     y - 24, 59,     y - 24);
    boxfill8(vram, COL8_FFFFFF, pitch, bpp,  2,     y - 24,  2,     y -  4);
    boxfill8(vram, COL8_848484, pitch, bpp,  3,     y -  4, 59,     y -  4);
    boxfill8(vram, COL8_848484, pitch, bpp, 59,     y - 23, 59,     y -  5);
    boxfill8(vram, COL8_000000, pitch, bpp,  2,     y -  3, 59,     y -  3);
    boxfill8(vram, COL8_000000, pitch, bpp, 60,     y - 24, 60,     y -  3);

    boxfill8(vram, COL8_848484, pitch, bpp, x - 47, y - 24, x -  4, y - 24);
    boxfill8(vram, COL8_848484, pitch, bpp, x - 47, y - 23, x - 47, y -  4);
    boxfill8(vram, COL8_FFFFFF, pitch, bpp, x - 47, y -  3, x -  4, y -  3);
    boxfill8(vram, COL8_FFFFFF, pitch, bpp, x -  3, y - 24, x -  3, y -  3);

    while(1)
        ;

    return 0;
}
