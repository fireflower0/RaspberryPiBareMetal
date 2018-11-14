#define MMIO_BASE       0x3F000000

/* mailbox message buffer */
volatile unsigned int  __attribute__((aligned(16))) mbox[36];

#define VIDEOCORE_MBOX  (MMIO_BASE+0x0000B880)
#define MBOX_READ       ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x00))
#define MBOX_POLL       ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x10))
#define MBOX_SENDER     ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x14))
#define MBOX_STATUS     ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x18))
#define MBOX_CONFIG     ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x1C))
#define MBOX_WRITE      ((volatile unsigned int*)(VIDEOCORE_MBOX + 0x20))

#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000

#define MBOX_REQUEST    0

/* channels */
#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8

/* tags */
#define MBOX_TAG_SETPOWER       0x28001
#define MBOX_TAG_SETCLKRATE     0x38002
#define MBOX_TAG_LAST           0

int mbox_call(unsigned char ch){
    unsigned int r = (((unsigned int)((unsigned long)&mbox) & ~0xF) | (ch & 0xF));

    /* メールボックスに書き込むまで待つ */
    do{
        asm volatile("nop");
    }while(*MBOX_STATUS & MBOX_FULL);

    /* メッセージのアドレスをチャネル識別子を持つメールボックスに書き込む */
    *MBOX_WRITE = r;

    while(1) {
        /* 応答を待つ */
        do{
            asm volatile("nop");
        }while(*MBOX_STATUS & MBOX_EMPTY);

        /* メッセージに対する応答か判定 */
        if(r == *MBOX_READ){
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

    if(mbox_call(MBOX_CH_PROP) && mbox[15] == 16 && mbox[23] != 0) {
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

void hline16(unsigned char *vram,
             unsigned int pitch,
             unsigned int bpp,
             int x,
             int y,
             unsigned int l,
             unsigned int w,
             unsigned int c) {
    unsigned short int *p = (unsigned short int *) coord2ptr(vram, pitch, bpp, x, y);
    if (w < l + x) {
        l = w - x;
    }

    for(unsigned int i = 0; i < l; i++) {
        *p++ = c;
    }
}

void vline16(unsigned char *vram,
             unsigned int pitch,
             unsigned int bpp,
             int x,
             int y,
             unsigned int l,
             unsigned int h,
             unsigned int c) {
    unsigned short int *p = (unsigned short int *) coord2ptr(vram, pitch, bpp, x, y);
    if (h < l + y) {
        l = h - y;
    }

    for(unsigned int i = 0; i < l; i++) {
        *p = c;
        p += pitch >> 1;
    }
}

int main(void){
    unsigned int i;
    fb_info_t fb_info = {1280, 800, 1280, 800, 0, 16, 0, 0, 0, 0};

    lfb_init(&fb_info);

    unsigned char *vram  = fb_info.buf_addr;
    unsigned int  pitch  = fb_info.row_bytes;
    unsigned int  bpp    = fb_info.bpp;
    unsigned int  width  = fb_info.display_w;
    unsigned int  height = fb_info.display_h;

    for(i = 0; i < height; i += 8) {
        hline16(vram, pitch, bpp, 0, i, width, width, 0x5050 * i);
    }

    for(i = 0; i < width; i += 8) {
        vline16(vram, pitch, bpp, i, 0, height, height, 0xa0a0 * i);
    }

    while(1)
        ;

    return 0;
}
