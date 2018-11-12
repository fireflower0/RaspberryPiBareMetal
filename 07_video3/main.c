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

unsigned int width, height, pitch, bpp;
unsigned char *lfb;

void lfb_init(){
    for(int i = 0; i < 36; i++){
        mbox[i] = 0;
    }

    mbox[0] = 112;
    mbox[1] = MBOX_REQUEST;

    mbox[2] = 0x48003;  //set phy wh
    mbox[3] = 8;
    mbox[4] = 0;
    mbox[5] = 1280;     //FrameBufferInfo.width
    mbox[6] = 800;     //FrameBufferInfo.height

    mbox[7] = 0x48004;  //set virt wh
    mbox[8] = 8;
    mbox[9] = 0;
    mbox[10] = 1280;     //FrameBufferInfo.virtual_width
    mbox[11] = 800;     //FrameBufferInfo.virtual_height
    
    mbox[12] = 0x48005; //set depth
    mbox[13] = 4;
    mbox[14] = 0;
    mbox[15] = 16;      //FrameBufferInfo.depth

    mbox[16] = 0x40008; //get pitch
    mbox[17] = 4;
    mbox[18] = 0;
    mbox[19] = 0;       //FrameBufferInfo.pitch

    mbox[20] = 0x40001; //get framebuffer, gets alignment on request
    mbox[21] = 8;
    mbox[22] = 0;
    mbox[23] = 16;      //FrameBufferInfo.pointer
    mbox[24] = 0;       //FrameBufferInfo.size

    mbox[25] = MBOX_TAG_LAST;

    if(mbox_call(MBOX_CH_PROP) && mbox[15] == 16 && mbox[23] != 0) {
        mbox[23] &= 0x3FFFFFFF;
        width  = mbox[5];
        height = mbox[6];
        bpp    = mbox[15];
        pitch  = mbox[19];
        lfb = (void*)((unsigned long)mbox[23]);
    }
}

static inline void *coord2ptr(int x, int y) {
    return (void *) (lfb + ((bpp + 7) >> 3) * x + pitch * y);
}

void boxfill8(unsigned int c, int x0, int y0, int x1, int y1){
    int x, y;
    unsigned short int *p;

    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++){
            p = (unsigned short int *) coord2ptr(x, y);
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
    lfb_init();

    int x = width;
    int y = height;

    boxfill8(COL8_008484,  0,     0,      x -  1, y - 29);
    boxfill8(COL8_C6C6C6,  0,     y - 28, x -  1, y - 28);
    boxfill8(COL8_FFFFFF,  0,     y - 27, x -  1, y - 27);
    boxfill8(COL8_C6C6C6,  0,     y - 26, x -  1, y -  1);

    boxfill8(COL8_FFFFFF,  3,     y - 24, 59,     y - 24);
    boxfill8(COL8_FFFFFF,  2,     y - 24,  2,     y -  4);
    boxfill8(COL8_848484,  3,     y -  4, 59,     y -  4);
    boxfill8(COL8_848484, 59,     y - 23, 59,     y -  5);
    boxfill8(COL8_000000,  2,     y -  3, 59,     y -  3);
    boxfill8(COL8_000000, 60,     y - 24, 60,     y -  3);

    boxfill8(COL8_848484, x - 47, y - 24, x -  4, y - 24);
    boxfill8(COL8_848484, x - 47, y - 23, x - 47, y -  4);
    boxfill8(COL8_FFFFFF, x - 47, y -  3, x -  4, y -  3);
    boxfill8(COL8_FFFFFF, x -  3, y - 24, x -  3, y -  3);

    while(1)
        ;

    return 0;
}
